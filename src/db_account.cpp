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
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::retrieveByCode()");

    DBCriteria criteria;
    criteria.add(Columns::code, DBCriteria::equal_to, code);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBAccount> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.at(0));

    log.exit("DBAccount::retrieveByCode()");
}

void DBAccount::createRecurringTransactions() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::createRecurringTransactions()");

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

        for (int i = 0;i < chargeResult.size();i++) {
            DBRecurringChargeView charge = chargeResult.at(i);

            if (charge.isActive()) {
                DBTransaction transaction;
                int numRows = transaction.findLatestByRecurringChargeID(charge.id);

                if (numRows == 0) {
                    log.debug("Found no exisiting transactions for charge %s", charge.id.c_str());

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
                    log.debug("Found latest transactions for charge %s with date '%s'", charge.id.c_str(), transaction.date.shortDate().c_str());
                    
                    transactionDate = charge.getNextRecurringTransactionDate(transaction.date);
                }

                log.debug(
                    "Got transaction date for charge %s - '%s' as '%s'", 
                    charge.id.c_str(), 
                    charge.description.c_str(), 
                    transactionDate.shortDate().c_str());

                while (transactionDate <= dateToday) {
                    if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
                        cout << "| " << transactionDate.shortDate() << " | " << charge.frequency.toString() << " | "
                            << setw(12) << charge.amount.localeFormattedStringValue() << " | " << charge.description << endl;
                    }

                    // Create the transaction
                    DBTransaction::createFromRecurringChargeAndDate(charge, transactionDate);

                    // Persist lastPaymentDate on the recurring charge (this makes it robust/idempotent)
                    charge.updateLastPaymentDate(transactionDate);
                    charge.retrieve();

                    // Move to next occurrence
                    transactionDate = charge.getNextRecurringTransactionDate(transactionDate);
                }
            }
        }

        db.commit();

        log.exit("DBAccount::createRecurringTransactions()");
    }
    catch (pfm_error & e) {
        db.rollback();

        log.error("DBAccount.createRecurringTransactions() - caught exception: %s", e.what());

        throw e;
    }
}

void DBAccount::beforeUpdate() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::beforeUpdate()");

    DBAccount account;
    account.retrieveByCode(code);

    if (openingBalance != account.openingBalance) {
        DBCarriedOver co;
        DBResult<DBCarriedOver> coResult = co.retrieveByAccountId(id);

        for (int i = 0;i < coResult.size();i++) {
            DBCarriedOver carriedOver = coResult.at(i);
            carriedOver.remove();
        }

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chResult = ch.retrieveByAccountID(id);

        for (int i = 0;i < chResult.size();i++) {
            DBRecurringCharge charge = chResult.at(i);

            DBTransaction tr;
            tr.deleteByRecurringChargeId(charge.id);
        }
    }

    log.exit("DBAccount::beforeUpdate()");
}

void DBAccount::doBalancePrerequisites() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::doBalancePrerequisites()");

    /*
    ** Make sure we're up to date, this will ensure that the
    ** balances are calculated correctly...
    */
    createRecurringTransactions();
    createCarriedOverLogs();

    log.exit("DBAccount::doBalancePrerequisites()");
}

void DBAccount::createCarriedOverLogs() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::createCarriedOverLogs()");

    StrDate dateToday;
    StrDate periodEndDate = dateToday.addMonths(-1).firstDayInMonth();

    log.debug("Using periodEndDate: %s", periodEndDate.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBCarriedOver co;
        int hasCO = co.retrieveLatestByAccountId(this->id);

        if (hasCO) {
            log.debug("Latest DBCarriedOverLog date '%s'", co.date.shortDate().c_str());
        }
        else {
            StrDate firstDate = openingDate;
            StrDate secondDate = openingDate.lastDayInMonth();

            co = DBCarriedOver::createForPeriod(this->id, openingBalance, firstDate, secondDate);
        }

        while (co.date < periodEndDate) {
            co.date = co.date.addMonths(1);

            StrDate firstDate = co.date.firstDayInMonth();
            StrDate secondDate = co.date.lastDayInMonth();

            log.debug("Creating DBCarriedOverLog for dates '%s' to '%s'", firstDate.shortDate().c_str(), secondDate.shortDate().c_str());
            
            co = DBCarriedOver::createForPeriod(this->id, co.balance, firstDate, secondDate);
        }

        db.commit();
        
        log.exit("DBAccount::createCarriedOverLogs()");
    }
    catch (pfm_error & e) {
        db.rollback();

        log.error("DBAccount.createCarriedOverLogs() - caught exception: %s", e.what());

        throw e;
    }
}

Money DBAccount::calculateCurrentBalance() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::calculateCurrentBalance()");

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

        log.debug(
                "calculateCurrentBalance(): Including carried over '%s' | '%s' | '%s'", 
                co.date.shortDate().c_str(), 
                co.description.c_str(), 
                co.balance.localeFormattedStringValue().c_str());
    }
    else {
        balance = openingBalance;
    }

    try {
        StrDate dateToday;
        StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(this->id, DBCriteria::ascending, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.size();i++) {
            DBTransaction transaction = transactionResult.at(i);

            log.debug(
                    "calculateCurrentBalance(): Including transaction '%s' | '%s' | '%s'", 
                    transaction.date.shortDate().c_str(), 
                    transaction.description.c_str(), 
                    transaction.amount.localeFormattedStringValue().c_str());

            balance += transaction.getSignedAmount();
        }

        log.exit("DBAccount::calculateCurrentBalance()");
    }
    catch (pfm_error & e) {
        log.error("DBAccount.calculateCurrentBalance() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}


Money DBAccount::calculateReconciledBalance() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::calculateReconciledBalance()");

    DBCarriedOver co;
    DBResult<DBCarriedOver> cos = co.retrieveByAccountId(this->id);

    Money balance = 0.00;

    StrDate dateToday;
    StrDate periodStartDate;
    DBCarriedOver referenceCO;

    /*
    ** If we have more than one carried over log, use the second to latest
    ** as it includes the account's opening balance. Otherwise, just use
    ** the account's opening balance.
    **
    ** Assume that all transactions prior to the chosen carried over log
    ** date should be reconciled by now, so go ahead and force that.
    */
    if (cos.size() > 1) {
        referenceCO = cos[1];
        balance = referenceCO.balance;

        periodStartDate = referenceCO.date;
        periodStartDate = periodStartDate.addDays(1);

        DBTransaction transaction;
        transaction.reconcileAllForAccountIDBeforeDate(this->id, referenceCO.date);

        log.debug(
                "calculateReconciledBalance(): Including carried over '%s' | '%s' | '%s'", 
                referenceCO.date.shortDate().c_str(), 
                referenceCO.description.c_str(), 
                referenceCO.balance.localeFormattedStringValue().c_str());
    }
    else {
        periodStartDate = openingDate;
        balance = openingBalance;
    }

    try {
        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveReconciledByAccountIDForPeriod(this->id, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.size();i++) {
            DBTransaction transaction = transactionResult.at(i);

            log.debug(
                    "calculateReconciledBalance(): Including transaction '%s' | '%s' | '%s'", 
                    transaction.date.shortDate().c_str(), 
                    transaction.description.c_str(), 
                    transaction.amount.localeFormattedStringValue().c_str());

            balance += transaction.getSignedAmount();
        }

        log.exit("DBAccount::calculateReconciledBalance()");
    }
    catch (pfm_error & e) {
        log.error("DBAccount.calculateReconciledBalance() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}

Money DBAccount::calculateBalanceAfterBills() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::calculateBalanceAfterBills()");

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

        log.debug(
                "calculateBalanceAfterBills(): Including carried over '%s' | '%s' | '%s'", 
                co.date.shortDate().c_str(), 
                co.description.c_str(), 
                co.balance.localeFormattedStringValue().c_str());
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
        DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(this->id, DBCriteria::ascending, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.size();i++) {
            DBTransaction transaction = transactionResult.at(i);

            log.debug(
                    "calculateBalanceAfterBills(): Including transaction '%s' | '%s' | '%s'", 
                    transaction.date.shortDate().c_str(), 
                    transaction.description.c_str(), 
                    transaction.amount.localeFormattedStringValue().c_str());

            balance += transaction.getSignedAmount();
            transactionBalance += transaction.getSignedAmount();
        }

        DBRecurringChargeView ch;
        DBResult<DBRecurringChargeView> chargeResult = ch.retrieveByAccountID(this->id);

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Identified charges due this period:" << endl;
        }

        for (int i = 0;i < chargeResult.size();i++) {
            DBRecurringCharge charge = chargeResult.at(i);

            if (charge.isChargeDueThisPeriod()) {
                log.debug(
                        "calculateBalanceAfterBills(): Including charge '%s' | '%s' | '%s'", 
                        charge.lastPaymentDate.shortDate().c_str(), 
                        charge.description.c_str(), 
                        charge.amount.localeFormattedStringValue().c_str());

                balance -= charge.amount;
                chargeBalance -= charge.amount;
            }
        }

        if (log.isLogLevel(LOG_LEVEL_DEBUG)) {
            cout << "Total transaction balance = " << transactionBalance.localeFormattedStringValue() << endl;
            cout << "Total charge balance = " << chargeBalance.localeFormattedStringValue() << endl;
        }

        log.exit("DBAccount::calculateBalanceAfterBills()");
    }
    catch (pfm_error & e) {
        log.error("DBAccount.calculateBalanceAfterBills() - caught exception: %s", e.what());

        throw e;
    }

    return balance;
}

bool DBAccount::isPrimary() {
    Logger & log = Logger::getInstance();
    log.entry("DBAccount::isPrimary()");

    string primaryAccountCode = DBPrimaryAccount::getPrimaryAccountCode();

    bool isPrimaryAccount = primaryAccountCode.compare(code) == 0 ? true : false;

    log.info("This account with code %s is %sthe primary account", this->code.c_str(), isPrimaryAccount ? "" : "not ");

    log.exit("DBAccount::isPrimary()");

    return isPrimaryAccount;
}
