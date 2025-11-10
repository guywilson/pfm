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

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t accountId, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.addOrderBy("date", dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("reconciled", true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveNonRecurringByAccountID(pfm_id_t accountId, DBCriteria::sql_order dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("recurring", false);
    criteria.addOrderBy("date", dateSortDirection);

    if (rowLimit > 0) {
        criteria.setRowLimit(rowLimit);
    }

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveNonRecurringByAccountID()");

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

    criteria.addOrderBy("date", DBCriteria::descending);
    criteria.setRowLimit(SQL_ROW_LIMIT);

    string statement = getSelectStatement() + criteria.getStatementCriteria();

    result.retrieve(statement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForAccountID(pfm_id_t accountId, DBCriteria & criteria) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::findTransactionsForAccountID()");

    criteria.addFirst("account_id", DBCriteria::equal_to, accountId);
    criteria.addOrderBy("date", DBCriteria::descending);
    criteria.setRowLimit(SQL_ROW_LIMIT);

    string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBTransactionView> result;
    result.retrieve(statement);

    log.exit("DBTransactionView::findTransactionsForAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountIDForPeriod(pfm_id_t accountId, DBCriteria::sql_order dateSortDirection, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate);
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate);
    criteria.addOrderBy("date", dateSortDirection);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("reconciled", true);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate);
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("recurring", false);
    criteria.add("date", DBCriteria::greater_than_or_equal, firstDate);
    criteria.add("date", DBCriteria::less_than_or_equal, secondDate);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    
    DBResult<DBTransactionView> result;

    result.retrieve(statement);

    log.exit("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::reportByCategory() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::reportByCategory()");

    DBResult<DBTransactionView> result;

    result.retrieve(sqlReportByCategoryNonRecurring);

    log.exit("DBTransactionView::reportByCategory()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::reportByPayee() {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::reportByPayee()");

    DBResult<DBTransactionView> result;

    result.retrieve(sqlReportByPayeeNonRecurring);

    log.exit("DBTransactionView::reportByPayee()");

    return result;
}
