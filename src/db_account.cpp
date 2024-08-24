#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_account.h"
#include "db_recurring_charge.h"
#include "db_transaction.h"
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

    int rowsRetrievedCount = result.executeSelect(szStatement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

void DBAccount::createRecurringTransactions() {
    StrDate dateToday;
    StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBRecurringCharge ch;
        DBResult<DBRecurringCharge> chargeResult = ch.retrieveByAccountID(this->id);

        for (int i = 0;i < chargeResult.getNumRows();i++) {
            DBRecurringCharge charge = chargeResult.getResultAt(i);

            DBTransaction transaction;
            transaction.date = charge.date;

            int chargeStatus = CHARGE_OK;

            while (transaction.date < periodStartDate && chargeStatus == CHARGE_OK) {
                transaction.findLatestByRecurringChargeID(charge.id);

                chargeStatus = charge.createNextTransactionForCharge(transaction.date);
            }
        }

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();

        fprintf(stderr, "DBAccount.createRecurringTransactions() - caught exception: %s\n", e.what());

        throw e;
    }
}

void DBAccount::createCarriedOverLogs() {
    StrDate dateToday;
    StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBCarriedOver co;
        int hasCO = co.retrieveLatestByAccountId(this->id);

        if (!hasCO) {
            DBTransaction transaction;
            DBResult<DBTransaction> result = transaction.retrieveByAccountID(this->id, sort_ascending, 1);

            co.date = result.getResultAt(0).date;
        }

        while (co.date < periodStartDate) {
            DBCarriedOver newCO(co);

            StrDate firstDate(co.date.year(), co.date.month(), 1);
            StrDate secondDate(co.date.year(), co.date.month(), co.date.daysInMonth());

            DBTransaction tr;
            DBResult<DBTransaction> transactionResult = tr.retrieveByAccountIDBetweenDates(this->id, firstDate, secondDate);

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
    }
    catch (pfm_error & e) {
        db.rollback();

        fprintf(stderr, "DBAccount.createCarriedOverLogs() - caught exception: %s\n", e.what());

        throw e;
    }
}

Money DBAccount::calculateBalanceAfterBills() {
    DBCarriedOver co;
    co.retrieveLatestByAccountId(this->id);

    Money balance = co.balance;

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        StrDate dateToday;
        StrDate periodStartDate(dateToday.year(), dateToday.month(), 1);

        DBTransaction tr;
        DBResult<DBTransaction> transactionResult = tr.retrieveByAccountIDBetweenDates(this->id, periodStartDate, dateToday);

        for (int i = 0;i < transactionResult.getNumRows();i++) {
            DBTransaction transaction = transactionResult.getResultAt(i);

            if (transaction.recurringChargeId == 0) {
                balance += transaction.getSignedAmount();
            }
        }

        DBRecurringCharge ch;
        DBResult<DBRecurringCharge> chargeResult = ch.retrieveByAccountID(this->id);

        for (int i = 0;i < chargeResult.getNumRows();i++) {
            DBRecurringCharge charge = chargeResult.getResultAt(i);

            if (charge.isChargeDueThisPeriod()) {
                balance -= charge.amount;
            }
        }

        db.commit();
    }
    catch (pfm_error & e) {
        db.rollback();

        fprintf(stderr, "DBAccount.calculateBalanceAfterBills() - caught exception: %s\n", e.what());

        throw e;
    }

    return balance;
}
