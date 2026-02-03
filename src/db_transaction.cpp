#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>

#include <sqlcipher/sqlite3.h>

#include "logger.h"
#include "pfm_error.h"
#include "db_base.h"
#include "db_payment.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_recurring_transfer.h"
#include "db_transaction.h"
#include "db_account.h"
#include "db_carried_over.h"
#include "db_transfer_transaction_record.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::isReconciled, true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBPayment::Columns::date, dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByRecurringChargeID(pfm_id_t & recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByRecurringChargeID()");

    DBCriteria criteria;
    criteria.add(Columns::recurringChargeId, DBCriteria::equal_to, recurringChargeId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByRecurringChargeID()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByRecurringChargeIDAfterDate(pfm_id_t & recurringChargeId, StrDate & after) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByRecurringChargeIDAfterDate()");

    DBCriteria criteria;
    criteria.add(Columns::recurringChargeId, DBCriteria::equal_to, recurringChargeId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, after);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByRecurringChargeIDAfterDate()");

    return result;
}

int DBTransaction::findLatestByRecurringChargeID(pfm_id_t & chargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::findLatestByRecurringChargeID()");

    DBCriteria criteria;
    criteria.add(Columns::recurringChargeId, DBCriteria::equal_to, chargeId);
    criteria.addOrderBy(DBPayment::Columns::date, DBCriteria::descending);
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

DBResult<DBTransaction> DBTransaction::findTransactionsForAccountID(pfm_id_t & accountId, string & criteria) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::findTransactionsForAccountID()");

    DBCriteria c;
    c.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    c.addOrderBy(DBPayment::Columns::date, DBCriteria::descending);
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

DBResult<DBTransaction> DBTransaction::retrieveByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::isReconciled, true);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::recurringChargeId, DBCriteria::is_null, recurringChargeId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> result;

    result.retrieve(statement);

    log.exit("DBTransaction::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}

void DBTransaction::reconcileAllForAccountIDBeforeDate(pfm_id_t & accountId, StrDate & referenceDate) {
    Logger & log = Logger::getInstance();

    log.entry("DBTransaction::reconcileAllForAccountIDBeforeDate()");

    string now = StrDate::getTimestamp();

    string updateStatement = 
            "UPDATE " + getTableName() + " SET " +
            Columns::isReconciled + " = 'Y', " +
            DBEntity::Columns::updatedDate + " = '" + now + "'";

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, referenceDate);
    criteria.add(Columns::isReconciled, false);

    string statement = updateStatement + criteria.getStatementCriteria();

    log.debug(
        "Reconciling all transactions before '%s' for account ID %s", 
        referenceDate.shortDate().c_str(), 
        accountId.c_str());

    PFM_DB & db = PFM_DB::getInstance();

    db.begin();

    try {
        db.executeUpdate(statement);
    }
    catch (pfm_error & e) {
        db.rollback();

        log.error("DBTransaction::reconcileAllForAccountIDBeforeDate() - Failed to execute update '%s'", e.what());

        throw e;
    }

    db.commit();

    log.exit("DBTransaction::reconcileAllForAccountIDBeforeDate()");
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
        tr.type = TYPE_DEBIT;
        tr.isReconciled = true;

        tr.save();

        if (src.isTransfer) {
            log.debug("Creating transfer records for charge '%s'", src.description.c_str());

            DBRecurringTransfer transfer;
            int transferExists = transfer.retrieveByRecurringChargeId(src.id);

            if (!transferExists) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Recurring charge ID(%s) is marked as a transfer but does not have a recurring transfer record!", src.id.c_str()), 
                        __FILE__, 
                        __LINE__);
            }
            
            DBAccount accountTo;
            accountTo.retrieve(transfer.accountToId);

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
        target.type = TYPE_CREDIT;
        target.accountId = accountTo.id;

        DBAccount accountFrom;
        accountFrom.id = source.accountId;
        accountFrom.retrieve();

        source.reference = "TR > " + accountTo.code;
        source.isTransfer = true;
        target.reference = "TR < " + accountFrom.code;
        target.isTransfer = true;

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

void DBTransaction::linkTransferTransactions() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::linkTransferTransactions()");

    log.debug("Attempting to link source & target transfer transactions");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::isTransfer, true);
    criteria.add(Columns::reference, DBCriteria::like, string("TR >%"));
    criteria.addOrderBy(DBPayment::Columns::accountId, DBCriteria::ascending);

    DBTransaction t;
    string statement = t.getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> sourceTransactions;
    sourceTransactions.retrieve(statement);

    criteria.clear();
    criteria.add(DBPayment::Columns::isTransfer, true);
    criteria.add(Columns::reference, DBCriteria::like, string("TR <%"));
    criteria.addOrderBy(DBPayment::Columns::accountId, DBCriteria::ascending);

    statement = t.getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransaction> targetTransactions;
    targetTransactions.retrieve(statement);

    if (sourceTransactions.size() != targetTransactions.size()) {
        throw pfm_validation_error("Failed to link transfer transactions, there should be an equal number of source and target transactions");
    }

    size_t matchCount = 0;

    for (size_t i = 0;i < sourceTransactions.size();i++) {
        DBTransaction source = sourceTransactions[i];

        for (size_t j = 0;j < targetTransactions.size();j++) {
            DBTransaction target = targetTransactions[j];

            if (target.categoryId.getValue() == source.categoryId.getValue() &&
                target.payeeId.getValue() == source.payeeId.getValue() &&
                target.date == source.date &&
                target.amount == source.amount)
            {
                log.debug(
                    "Found match on categoryId, payeeId, date '%s' and amount %s for transaction '%s'", 
                    source.date.shortDate().c_str(), 
                    source.amount.rawStringValue().c_str(), 
                    source.description.c_str());

                DBAccount targetAccount;
                targetAccount.retrieve(target.accountId);

                DBAccount sourceAccount;
                sourceAccount.retrieve(source.accountId);

                if (targetAccount.code == source.reference.substr(5) &&
                    sourceAccount.code == target.reference.substr(5))
                {
                    matchCount++;

                    log.debug(
                        "Found match on source account '%s' to target account '%s'", 
                        sourceAccount.code.c_str(), 
                        targetAccount.code.c_str());

                    DBTransferTransactionRecord transfer;
                    int isExistingTransferPresent = transfer.retrieveByTransactionIds(source.id, target.id);

                    if (!isExistingTransferPresent) {
                        DBTransferTransactionRecord::createFromTransactions(target, source);
                    }
                }
            }
        }
    }

    log.debug("Found a total of %d matches for %d transfer transactions", matchCount, sourceTransactions.size());

    if (matchCount != sourceTransactions.size()) {
        throw pfm_validation_error("Failed to link transfer transactions, failed to match all source and target transactions");
    }

    log.exit("DBTransaction::linkTransferTransactions()");
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

    for (size_t i = 0;i < coResult.size();i++) {
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

        for (size_t i = 0;i < coResult.size();i++) {
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

    for (size_t i = 0;i < coResult.size();i++) {
        DBCarriedOver carriedOver = coResult.at(i);

        carriedOver.balance -= getSignedAmount();
        carriedOver.save();
    }

    log.exit("DBTransaction::afterRemove()");
}

void DBTransaction::deleteByRecurringChargeId(pfm_id_t & recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::deleteByRecurringChargeId()");

    DBCriteria criteria;
    criteria.add(Columns::recurringChargeId, DBCriteria::equal_to, recurringChargeId);

    string statement = getDeleteStatement() + ' ' + criteria.getStatementCriteria();

    remove(statement);

    log.exit("DBTransaction::deleteByRecurringChargeId()");
}

void DBTransaction::deleteAllRecurringTransactionsForAccount(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransaction::deleteAllRecurringTransactionsForAccount()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::recurringChargeId, DBCriteria::is_not_null, recurringChargeId);

    string statement = getDeleteStatement() + ' ' + criteria.getStatementCriteria();

    remove(statement);

    log.exit("DBTransaction::deleteAllRecurringTransactionsForAccount()");
}
