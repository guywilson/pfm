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
#include "db_primary_account.h"
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

                log.logDebug(
                    "Got transaction date for charge %lld - '%s' as '%s'", 
                    charge.id, 
                    charge.description.c_str(), 
                    transactionDate.shortDate().c_str());

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
    }
}

void DBAccount::createCarriedOverLogs() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::createCarriedOverLogs()");

    StrDate dateToday;
    StrDate periodEndDate = dateToday.addMonths(-1).firstDayInMonth();

    log.logDebug("Using periodEndDate: %s", periodEndDate.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBCarriedOver co;
        int hasCO = co.retrieveLatestByAccountId(this->id);

        if (hasCO) {
            log.logDebug("Latest DBCarriedOverLog date '%s'", co.date.shortDate().c_str());
        }
        else {
            DBTransactionView transaction;
            DBResult<DBTransactionView> result = transaction.retrieveByAccountID(this->id, sort_ascending, 1);

            if (result.getNumRows() == 0) {
                return;
            }
            
            DBTransaction firstTransaction = result.getResultAt(0);

            StrDate firstDate = firstTransaction.date.firstDayInMonth();
            StrDate secondDate = firstTransaction.date.lastDayInMonth();

            DBCarriedOver newCo;
            newCo.createForPeriod(this->id, openingBalance, firstDate, secondDate);

            co = newCo;
        }

        while (co.date < periodEndDate) {
            co.date = co.date.addMonths(1);

            StrDate firstDate = co.date.firstDayInMonth();
            StrDate secondDate = co.date.lastDayInMonth();

            log.logDebug("Creating DBCarriedOverLog for dates '%s' to '%s'", firstDate.shortDate().c_str(), secondDate.shortDate().c_str());
            
            DBCarriedOver newCo;
            newCo.createForPeriod(this->id, co.balance, firstDate, secondDate);

            co = newCo;
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

Money DBAccount::calculateCurrentBalance() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::calculateCurrentBalance()");

    DBCarriedOver co;
    int numCORecords = co.retrieveLatestByAccountId(this->id);

    Money balance = 0.00;

    /*
    ** If we have a carried over log, use that as our starting balance
    ** as it includes the account's opening balance. Otherwise, just use
    ** the account's opening balance.
    */
    if (numCORecords) {
        balance = co.balance;

        log.logDebug(
                "calculateCurrentBalance(): Including carried over '%s' | '%s' | '%s'", 
                co.date.shortDate().c_str(), 
                co.description.c_str(), 
                co.balance.getFormattedStringValue().c_str());
    }
    else {
        balance = openingBalance;
    }

    try {
        StrDate dateToday;
        StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(this->id, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.getNumRows();i++) {
            DBTransaction transaction = transactionResult.getResultAt(i);

            log.logDebug(
                    "calculateCurrentBalance(): Including transaction '%s' | '%s' | '%s'", 
                    transaction.date.shortDate().c_str(), 
                    transaction.description.c_str(), 
                    transaction.amount.getFormattedStringValue().c_str());

            balance += transaction.getSignedAmount();
        }

        log.logExit("DBAccount::calculateCurrentBalance()");
    }
    catch (pfm_error & e) {
        log.logError("DBAccount.calculateCurrentBalance() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}

Money DBAccount::calculateBalanceAfterBills() {
    Logger & log = Logger::getInstance();
    log.logEntry("DBAccount::calculateBalanceAfterBills()");

    DBCarriedOver co;
    int numCORecords = co.retrieveLatestByAccountId(this->id);

    Money balance = 0.00;

    /*
    ** If we have a carried over log, use that as our starting balance
    ** as it includes the account's opening balance. Otherwise, just use
    ** the account's opening balance.
    */
    if (numCORecords) {
        balance = co.balance;

        log.logDebug(
                "calculateBalanceAfterBills(): Including carried over '%s' | '%s' | '%s'", 
                co.date.shortDate().c_str(), 
                co.description.c_str(), 
                co.balance.getFormattedStringValue().c_str());
    }
    else {
        balance = openingBalance;
    }

    Money transactionBalance = 0.0;
    Money chargeBalance = 0.0;

    try {
        StrDate dateToday;
        StrDate periodStartDate = dateToday.firstDayInMonth();

        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(this->id, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.getNumRows();i++) {
            DBTransaction transaction = transactionResult.getResultAt(i);

            log.logDebug(
                    "calculateBalanceAfterBills(): Including transaction '%s' | '%s' | '%s'", 
                    transaction.date.shortDate().c_str(), 
                    transaction.description.c_str(), 
                    transaction.amount.getFormattedStringValue().c_str());

            balance += transaction.getSignedAmount();
            transactionBalance += transaction.getSignedAmount();
        }

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chargeResult = ch.retrieveByAccountID(this->id);

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Identified charges due this period:" << endl;
        }

        for (int i = 0;i < chargeResult.getNumRows();i++) {
            DBRecurringCharge charge = chargeResult.getResultAt(i);

            if (charge.isChargeDueThisPeriod(dateToday)) {
                log.logDebug(
                        "calculateBalanceAfterBills(): Including charge '%s' | '%s' | '%s'", 
                        charge.nextPaymentDate.shortDate().c_str(), 
                        charge.description.c_str(), 
                        charge.amount.getFormattedStringValue().c_str());

                balance -= charge.amount;
                chargeBalance -= charge.amount;
            }
        }

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Total transaction balance = " << transactionBalance.getFormattedStringValue() << endl;
            cout << "Total charge balance = " << chargeBalance.getFormattedStringValue() << endl;
        }

        log.logExit("DBAccount::calculateBalanceAfterBills()");
    }
    catch (pfm_error & e) {
        log.logError("DBAccount.calculateBalanceAfterBills() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}

bool DBAccount::isPrimary() {
    string primaryAccountCode = DBPrimaryAccount::getPrimaryAccountCode();

    return primaryAccountCode.compare(code) == 0 ? true : false;
}
