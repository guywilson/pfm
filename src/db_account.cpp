#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_account.h"
#include "db_recurring_charge.h"
#include "db_v_recurring_charge.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "db_carried_over.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBAccount::retrieveByCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBAccount> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCode, 
        code.c_str());

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

void DBAccount::createRecurringTransactions() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::createRecurringTransactions()");

    StrDate dateToday;
    StrDate transactionDate;

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chargeResult = ch.retrieveByAccountID(this->id);

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Creating recurring transactions for account '" << this->code << "': " << endl;
        }

        for (int i = 0;i < chargeResult.getNumRows();i++) {
            DBRecurringChargeView charge = chargeResult.getResultAt(i);

            if (charge.isActive()) {
                DBTransaction transaction;
                int numRows = transaction.findLatestByRecurringChargeID(charge.id);

                if (numRows == 0) {
                    log.logDebug("Found no exisiting transactions for charge %lld", charge.id);

                    if (charge.date < openingDate) {
                        transactionDate = charge.date;

                        while (transactionDate < openingDate) {
                            transactionDate = charge.getNextRecurringTransactionDate(transactionDate);
                        }
                    }
                    else {
                        transactionDate = charge.date;
                    }
                }
                else {
                    log.logDebug("Found latest transactions for charge %lld with date '%s'", charge.id, transaction.date.shortDate().c_str());
                    
                    transactionDate = charge.getNextRecurringTransactionDate(transaction.date);
                }

                while (transactionDate <= dateToday) {
                    if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
                        cout << "| " << transactionDate.shortDate() << " | " << charge.frequency << " | " << setw(16) << right << charge.amount.getFormattedStringValue() << " | " << charge.description << endl;
                    }

                    transaction.createFromRecurringChargeAndDate(charge, transactionDate);
                    transactionDate = charge.getNextRecurringTransactionDate(transactionDate);
                }
            }
        }

        db.commit();

        log.logExit("DBAccount::createRecurringTransactions()");
    }
    catch (pfm_error & e) {
        db.rollback();

        log.logError("DBAccount.createRecurringTransactions() - caught exception: %s", e.what());

        throw e;
    }
}

void DBAccount::beforeUpdate() {
    DBAccount account;
    account.retrieveByCode(code);

    if (openingBalance != account.openingBalance) {
        DBCarriedOver co;
        DBResult<DBCarriedOver> coResult = co.retrieveByAccountId(id);

        for (int i = 0;i < coResult.getNumRows();i++) {
            DBCarriedOver carriedOver = coResult.getResultAt(i);
            carriedOver.remove();
        }

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chResult = ch.retrieveByAccountID(id);

        for (int i = 0;i < chResult.getNumRows();i++) {
            DBRecurringCharge charge = chResult.getResultAt(i);

            DBTransaction tr;
            tr.deleteByRecurringChargeId(charge.id);
        }

        currentBalance = openingBalance;
    }
}

void DBAccount::createCarriedOverLogs() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::createCarriedOverLogs()");

    StrDate dateToday;
    StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBCarriedOver co;
        int hasCO = co.retrieveLatestByAccountId(this->id);

        if (!hasCO) {
            DBTransactionView transaction;
            DBResult<DBTransactionView> result = transaction.retrieveByAccountID(this->id, sort_ascending, 1);

            if (result.getNumRows() == 0) {
                return;
            }
            
            StrDate latestTransactionDate = result.getResultAt(0).date;

            co.date = StrDate(latestTransactionDate.year(), latestTransactionDate.month(), 1);
            co.balance = openingBalance;
        }

        while (co.date < periodStartDate) {
            StrDate firstDate(co.date.year(), co.date.month(), 1);
            StrDate secondDate(co.date.year(), co.date.month(), co.date.daysInMonth());

            DBTransactionView tr;
            DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(this->id, firstDate, secondDate);

            DBCarriedOver newCO(co);

            for (int i = 0;i < transactionResult.getNumRows();i++) {
                DBTransaction transaction = transactionResult.getResultAt(i);

                newCO.balance += transaction.getSignedAmount();
            }

            /*
            ** Clear the ID so we insert a new record in save()...
            */
            newCO.id = 0;

            newCO.accountId = this->id;
            newCO.date.addMonths(1);
            newCO.description = "Carried over (" + newCO.date.shortDate() + ")";

            co = newCO;

            newCO.save();
        }

        db.commit();
        
        log.logExit("DBAccount::createCarriedOverLogs()");
    }
    catch (pfm_error & e) {
        db.rollback();

        log.logError("DBAccount.createCarriedOverLogs() - caught exception: %s", e.what());

        throw e;
    }
}

Money DBAccount::calculateBalanceAfterBills() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::calculateBalanceAfterBills()");

    DBCarriedOver co;
    co.retrieveLatestByAccountId(this->id);

    Money balance = co.balance;

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        StrDate dateToday;
        StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveNonRecurringByAccountIDForPeriod(this->id, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.getNumRows();i++) {
            DBTransaction transaction = transactionResult.getResultAt(i);
            balance += transaction.getSignedAmount();
        }

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chargeResult = ch.retrieveByAccountID(this->id);

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Identified charges due this period:" << endl;
        }

        for (int i = 0;i < chargeResult.getNumRows();i++) {
            DBRecurringCharge charge = chargeResult.getResultAt(i);

            if (charge.isChargeDueThisPeriod(dateToday.year(), dateToday.month())) {
                if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
                    cout << "| " << charge.date.shortDate() << " | " << charge.frequency << " | " << setw(16) << right << charge.amount.getFormattedStringValue() << " | " << charge.description << endl;
                }
                balance -= charge.amount;
            }
        }

        db.commit();

        log.logExit("DBAccount::calculateBalanceAfterBills()");
    }
    catch (pfm_error & e) {
        db.rollback();

        log.logError("DBAccount.calculateBalanceAfterBills() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}
