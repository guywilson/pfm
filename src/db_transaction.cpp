#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "logger.h"
#include "pfm_error.h"
#include "db_base.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_account.h"
#include "db_carried_over.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBTransaction> DBTransaction::retrieveByStatementAndID(const char * sqlSelect, pfm_id_t id) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelect, 
        id);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectByAccountID, accountId);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountID()");

    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectReconciledByAccountID, accountId);

    log.exit("DBTransaction::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDSortedByDate, 
        accountId,
        (dateSortDirection == sort_ascending ? "ASC" : "DESC"));

    if (rowLimit > 0) {
        char szLimit[LIMIT_CLAUSE_BUFFER_LEN];

        snprintf(szLimit, LIMIT_CLAUSE_BUFFER_LEN, " LIMIT %d;", rowLimit);
        strcat(szStatement, szLimit);
    }
    else {
        strcat(szStatement, ";");
    }

    result.retrieve(szStatement);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByRecurringChargeID(pfm_id_t recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByRecurringChargeID()");

    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectByRecurringChargeID, recurringChargeId);

    log.exit("DBTransaction::retrieveByRecurringChargeID()");

    return result;
}

int DBTransaction::findLatestByRecurringChargeID(pfm_id_t chargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::findLatestByRecurringChargeID()");

    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectLatestByChargeID, chargeId);

    if (result.size() == 1) {
        set(result.at(0));
    }
    else if (result.size() > 1) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Expected no more than 1 row, got %d", 
                    result.size()), 
                __FILE__, 
                __LINE__);
    }

    log.exit("DBTransaction::findLatestByRecurringChargeID()");

    return result.size();
}

DBResult<DBTransaction> DBTransaction::findTransactionsForAccountID(pfm_id_t accountId, string & criteria) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::findTransactionsForAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int sqlRowLimit = SQL_ROW_LIMIT;
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    if (criteria.length() > 0) {
        /*
        ** Remove the trailing ';' from the statement...
        */
        szStatement[strlen(szStatement) - 1] = 0;

        strcat(szStatement, " AND ");
        strcat(szStatement, criteria.c_str());
    }
    
    snprintf(
        &szStatement[strlen(szStatement)], 
        SQL_STATEMENT_BUFFER_LEN, 
        " ORDER BY date DESC LIMIT %d;", 
        sqlRowLimit);

    result.retrieve(szStatement);

    log.exit("DBTransaction::findTransactionsForAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransaction::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectReconciledByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectNonRecurringByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}

void DBTransaction::reconcileAllForAccountIDBeforeDate(pfm_id_t accountId, StrDate & referenceDate) {
    Logger & log = Logger::getInstance();

    log.entry("reconcileAllForAccountIDBeforeDate()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::getTimestamp();

    log.debug(
        "Reconciling all transactions before '%s' for account ID %lld", 
        referenceDate.shortDate().c_str(), 
        accountId);

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlReconcileByAccountIDBeforeDate, 
        now.c_str(),
        accountId,
        referenceDate.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    db.begin();

    try {
        db.executeUpdate(szStatement);
    }
    catch (pfm_error & e) {
        db.rollback();

        log.error("reconcileAllForAccountIDBeforeDate() - Failed to execute update '%s'", e.what());

        throw e;
    }

    db.commit();

    log.exit("reconcileAllForAccountIDBeforeDate");
}

void DBTransaction::createTransferSource(const DBRecurringCharge & src, StrDate & transactionDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createTransferSource()");

    DBTransaction transaction;

    transaction.setFromRecurringCharge(src);

    transaction.date = transactionDate;
    transaction.isCredit = false;
    transaction.isReconciled = true;

    DBAccount account;
    account.id = src.transferToAccountId;

    account.retrieve();

    transaction.reference = "TR > " +  account.code;

    transaction.save();

    log.exit("DBTransaction::createTransferSource()");
}

void DBTransaction::createTransferTarget(const DBRecurringCharge & src, StrDate & transactionDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createTransferTarget()");

    DBTransaction transaction;

    transaction.setFromRecurringCharge(src);

    transaction.accountId = src.transferToAccountId;
    transaction.date = transactionDate;
    transaction.isCredit = true;
    transaction.isReconciled = true;

    DBAccount account;
    account.id = src.accountId;

    account.retrieve();

    transaction.reference = "TR < " +  account.code;

    transaction.save();

    log.exit("DBTransaction::createTransferTarget()");
}

void DBTransaction::createFromRecurringChargeAndDate(const DBRecurringCharge & src, StrDate & transactionDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createFromRecurringChargeAndDate()");

    log.debug("Creating recurring transaction '%s' for date '%s'", src.description.c_str(), transactionDate.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        if (src.transferToAccountId != 0) {
            log.debug("Creating transfer transactions for recurring charge '%s'", src.description.c_str());

            createTransferSource(src, transactionDate);
            createTransferTarget(src, transactionDate);
        }
        else {
            DBTransaction tr;
            tr.setFromRecurringCharge(src);

            tr.date = transactionDate;
            tr.isCredit = false;
            tr.isReconciled = true;

            tr.save();
        }

        db.commit();
    }
    catch (exception & e) {
        log.error("DBTransaction::createFromRecurringChargeAndDate() - Error '%s'", e.what());
        throw pfm_error(pfm_error::buildMsg("Failed to create transaction from recurring charge - '%s'", e.what()));
    }

    log.exit("DBTransaction::createFromRecurringChargeAndDate()");
}

int DBTransaction::createNextTransactionForCharge(DBRecurringCharge & charge, StrDate & latestDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createNextTransactionForCharge()");

    StrDate dateToday;

    if (latestDate <= dateToday) {
        StrDate nextPaymentDate = latestDate;

        char frequencyValue = charge.getFrequencyValue();
        char frequencyUnit = charge.getFrequencyUnit();

        switch (frequencyUnit) {
            case 'y':
                nextPaymentDate.addYears(frequencyValue);
                break;

            case 'm':
                nextPaymentDate.addMonths(frequencyValue);
                break;

            case 'w':
                nextPaymentDate.addWeeks(frequencyValue);
                break;

            case 'd':
                nextPaymentDate.addDays(frequencyValue);
                break;
        }

        if (nextPaymentDate <= dateToday) {
            DBTransaction transaction;
            transaction.createFromRecurringChargeAndDate(charge, nextPaymentDate);

            return CHARGE_OK;
        }
    }

    log.exit("DBTransaction::createNextTransactionForCharge()");

    return CHARGE_NOT_DUE;
}

void DBTransaction::afterInsert() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::afterInsert()");

    DBCarriedOver co;
    DBResult<DBCarriedOver> coResult = co.retrieveByAccountIdAfterDate(accountId, date);

    for (int i = 0;i < coResult.size();i++) {
        DBCarriedOver carriedOver = coResult.at(i);

        carriedOver.balance += getSignedAmount();
        carriedOver.save();
    }

    log.exit("DBTransaction::afterInsert()");
}

void DBTransaction::beforeUpdate() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::beforeUpdate()");

    DBTransaction transaction;
    transaction.retrieve(id);

    /*
    ** If the amount has been updated on the transaction,
    ** revert and re-add the transaction amount to the 
    ** current balance...
    */
    if (this->getSignedAmount() != transaction.getSignedAmount()) {
        DBCarriedOver co;
        DBResult<DBCarriedOver> coResult = co.retrieveByAccountIdAfterDate(accountId, date);

        for (int i = 0;i < coResult.size();i++) {
            DBCarriedOver carriedOver = coResult.at(i);

            log.debug("Updating CO with date '%s'", carriedOver.date.shortDate().c_str());

            carriedOver.balance -= transaction.getSignedAmount();
            carriedOver.balance += this->getSignedAmount();
            carriedOver.save();
        }
    }

    log.exit("DBTransaction::beforeUpdate()");
}

void DBTransaction::afterRemove() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::afterRemove()");

    DBCarriedOver co;
    DBResult<DBCarriedOver> coResult = co.retrieveByAccountIdAfterDate(accountId, date);

    for (int i = 0;i < coResult.size();i++) {
        DBCarriedOver carriedOver = coResult.at(i);

        carriedOver.balance -= getSignedAmount();
        carriedOver.save();
    }

    log.exit("DBTransaction::afterRemove()");
}

void DBTransaction::deleteByRecurringChargeId(pfm_id_t recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::deleteByRecurringChargeId()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlDeleteByRecurringCharge, 
        recurringChargeId);

    remove(szStatement);

    log.exit("DBTransaction::deleteByRecurringChargeId()");
}

void DBTransaction::deleteAllRecurringTransactionsForAccount(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::deleteAllRecurringTransactionsForAccount()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlDeleteAllRecurringForAccount, 
        accountId);

    remove(szStatement);

    log.exit("DBTransaction::deleteAllRecurringTransactionsForAccount()");
}
