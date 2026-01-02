#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "logger.h"
#include "pfm_error.h"
#include "db_base.h"
#include "db_v_transaction.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBPayment::Columns::date, dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::listByAccountID(pfm_id_t & accountId, bool includeRecurring, bool thisPeriod, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);

    if (!includeRecurring) {
        criteria.add(Columns::recurring, false);
    }

    if (thisPeriod) {
        StrDate today;
        StrDate periodStart = StrDate::getPeriodStartDate(today);

        criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, periodStart);
        criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, today);
    }

    criteria.addOrderBy(DBPayment::Columns::date, dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveNonRecurringByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::isReconciled, true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactions(const string & sql) {
    DBResult<DBTransactionView> result;

    string statement = getSelectStatement() + " WHERE " + sql;

    result.retrieve(statement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForCriteria(DBCriteria & criteria) {
    DBResult<DBTransactionView> result;

    criteria.addOrderBy(DBPayment::Columns::date, DBCriteria::descending);
    criteria.setRowLimit(SQL_ROW_LIMIT);

    string statement = getSelectStatement() + criteria.getStatementCriteria();

    result.retrieve(statement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForAccountID(pfm_id_t & accountId, DBCriteria & criteria) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::findTransactionsForAccountID()");

    criteria.addFirst(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBPayment::Columns::date, DBCriteria::descending);
    criteria.setRowLimit(SQL_ROW_LIMIT);

    string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBTransactionView> result;
    result.retrieve(statement);

    log.exit("DBTransactionView::findTransactionsForAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountIDForPeriod(pfm_id_t & accountId, DBCriteria::sql_order dateSortDirection, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);
    criteria.addOrderBy(DBPayment::Columns::date, dateSortDirection);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::isReconciled, true);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t & accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::recurring, false);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, firstDate);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::reportByCategory() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::reportByCategory()");

    string statement = "SELECT ";

    statement.append(Columns::category);
    statement.append(", ");
    statement.append("SUM(");
    statement.append(DBPayment::Columns::amount);
    statement.append(") AS total ");
    statement.append("FROM ");
    statement.append(getTableName());

    DBCriteria criteria;
    criteria.add(Columns::recurring, false);
    criteria.add(Columns::type, DBCriteria::equal_to, string(TYPE_DEBIT));
    criteria.addGroupBy(Columns::category);

    statement += criteria.getStatementCriteria();

    DBResult<DBTransactionView> result;
    result.retrieve(statement);

    log.exit("DBTransactionView::reportByCategory()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::reportByPayee() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::reportByPayee()");

    string statement = "SELECT ";

    statement.append(Columns::payee);
    statement.append(", ");
    statement.append("SUM(");
    statement.append(DBPayment::Columns::amount);
    statement.append(") AS total ");
    statement.append("FROM ");
    statement.append(getTableName());

    DBCriteria criteria;
    criteria.add(Columns::recurring, false);
    criteria.add(Columns::type, DBCriteria::equal_to, string(TYPE_DEBIT));
    criteria.addGroupBy(Columns::payee);

    statement += criteria.getStatementCriteria();

    DBResult<DBTransactionView> result;
    result.retrieve(statement);

    log.exit("DBTransactionView::reportByPayee()");

    return result;
}
