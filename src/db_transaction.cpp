#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>

#include <sqlcipher/sqlite3.h>

#include "logger.h"
#include "pfm_error.h"
#include "db_base.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_account.h"
#include "db_carried_over.h"
#include "db_transfer_transaction_record.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("is_reconciled", true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t accountId, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.addOrderBy("date", dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByRecurringChargeID(pfm_id_t recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByRecurringChargeID()");

    DBCriteria criteria;
    criteria.add("recurring_charge_id", DBCriteria::equal_to, recurringChargeId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByRecurringChargeID()");

    return result;
}

int DBTransaction::findLatestByRecurringChargeID(pfm_id_t chargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::findLatestByRecurringChargeID()");

    DBCriteria criteria;
    criteria.add("recurring_charge_id", DBCriteria::equal_to, chargeId);
    criteria.addOrderBy("date", DBCriteria::descending);
    criteria.setRowLimit(1);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

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

    DBCriteria c;
    c.add("account_id", DBCriteria::equal_to, accountId);
    c.addOrderBy("date", DBCriteria::descending);
    c.setRowLimit(SQL_ROW_LIMIT);

    string statement = getSelectStatement() + c.getWhereClause();

    if (criteria.length() > 0) {
        statement.append(" AND ");
        statement.append(criteria);
        statement.append(c.getOrderBy());
        statement.append(c.getLimitClause());
    }

    DBResult<DBTransaction> result;
    result.retrieve(statement);

    log.exit("DBTransaction::findTransactionsForAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate.shortDate());
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate.shortDate());

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("is_reconciled", true);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate.shortDate());
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate.shortDate());

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("recurring_charge_id", DBCriteria::is_null);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate.shortDate());
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate.shortDate());

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}

void DBTransaction::reconcileAllForAccountIDBeforeDate(pfm_id_t accountId, StrDate & referenceDate) {
    Logger & log = Logger::getInstance();

    log.entry("reconcileAllForAccountIDBeforeDate()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];

    string now = StrDate::getTimestamp();

    log.debug(
        "Reconciling all transactions before '%s' for account ID %s", 
        referenceDate.shortDate().c_str(), 
        accountId.c_str());

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlReconcileByAccountIDBeforeDate, 
        now.c_str(),
        accountId.c_str(),
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

void DBTransaction::createFromRecurringChargeAndDate(DBRecurringCharge & src, StrDate & transactionDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createFromRecurringChargeAndDate()");

    log.debug("Creating recurring transaction '%s' for date '%s'", src.description.c_str(), transactionDate.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBTransaction tr;
        tr.setFromRecurringCharge(src);

        tr.date = transactionDate;
        tr.isCredit = false;
        tr.isReconciled = true;

        tr.save();

        if (src.isTransfer()) {
            log.debug("Creating transfer records for charge '%s'", src.description.c_str());

            DBAccount accountTo;
            accountTo.id = src.transfer.accountToId;
            accountTo.retrieve();

            DBTransaction::createTransferPairFromSource(tr, accountTo);
        }

        db.commit();
    }
    catch (exception & e) {
        log.error("Failed to create transaction from recurring charge: %s", e.what());
        db.rollback();
    }

    log.exit("DBTransaction::createFromRecurringChargeAndDate()");
}

void DBTransaction::createTransferPairFromSource(DBTransaction & source, DBAccount & accountTo) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createTransferPairFromSource()");

    log.debug("Creating transfer pair '%s' for date '%s'", source.description.c_str(), source.date.shortDate().c_str());

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        DBTransaction target;
        target.set(source);

        target.id.clear();
        target.isCredit = true;
        target.accountId = accountTo.id;

        DBAccount accountFrom;
        accountFrom.id = source.accountId;
        accountFrom.retrieve();

        source.reference = "TR > " + accountTo.code;
        target.reference = "TR < " + accountFrom.code;

        target.save();
        source.save();

        DBTransferTransactionRecord::createFromTransactions(target, source);

        db.commit();
    }
    catch (exception & e) {
        log.error("Failed to create transfer transaction pair: %s", e.what());
        db.rollback();
    }

    log.exit("DBTransaction::createTransferPairFromSource()");
}

int DBTransaction::createNextTransactionForCharge(DBRecurringCharge & charge, StrDate & latestDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::createNextTransactionForCharge()");

    StrDate dateToday;

    if (latestDate <= dateToday) {
        StrDate nextPaymentDate = latestDate;

        char frequencyValue = charge.frequency.count;
        FrequencyUnit frequencyUnit = charge.frequency.unit;

        switch (frequencyUnit) {
            case FrequencyUnit::Years:
                nextPaymentDate.addYears(frequencyValue);
                break;

            case FrequencyUnit::Months:
                nextPaymentDate.addMonths(frequencyValue);
                break;

            case FrequencyUnit::Weeks:
                nextPaymentDate.addWeeks(frequencyValue);
                break;

            case FrequencyUnit::Days:
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

    DBCriteria criteria;
    criteria.add("recurring_charge_id", DBCriteria::equal_to, recurringChargeId);

    string statement = getDeleteStatement() + ' ' + criteria.getStatementCriteria();

    remove(statement);

    log.exit("DBTransaction::deleteByRecurringChargeId()");
}

void DBTransaction::deleteAllRecurringTransactionsForAccount(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::deleteAllRecurringTransactionsForAccount()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("recurring_charge_id", DBCriteria::is_not_null);

    string statement = getDeleteStatement() + ' ' + criteria.getStatementCriteria();

    remove(statement);

    log.exit("DBTransaction::deleteAllRecurringTransactionsForAccount()");
}
