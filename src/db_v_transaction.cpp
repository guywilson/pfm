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

DBResult<DBTransactionView> DBTransactionView::retrieveByStatementAndID(const char * sqlSelect, pfm_id_t id) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelect, 
        id);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    DBResult<DBTransactionView> result = retrieveByStatementAndID(sqlSelectByAccountID, accountId);

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

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

    log.exit("DBTransactionView::retrieveByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountID()");

    DBResult<DBTransactionView> result = retrieveByStatementAndID(sqlSelectReconciledByAccountID, accountId);

    log.exit("DBTransactionView::retrieveReconciledByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveNonRecurringByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectNonRecurringByAccountIDSortedByDate, 
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

    log.exit("DBTransactionView::retrieveNonRecurringByAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForCriteria(const string & criteria) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int sqlRowLimit = SQL_ROW_LIMIT;
    DBResult<DBTransactionView> result;

    strncpy(szStatement, sqlSelectByCriteria, SQL_STATEMENT_BUFFER_LEN - 1);

    if (criteria.length() > 0) {
        strcat(szStatement, criteria.c_str());
    }
    
    char szOrderBy[SQL_ORDER_BY_BUFFER_LEN];

    snprintf(
        szOrderBy, 
        SQL_ORDER_BY_BUFFER_LEN - 1, 
        " ORDER BY date DESC LIMIT %d;", 
        sqlRowLimit);

    strcat(szStatement, szOrderBy);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForAccountID(pfm_id_t accountId, const string & criteria) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::findTransactionsForAccountID()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int sqlRowLimit = SQL_ROW_LIMIT;
    DBResult<DBTransactionView> result;

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
        
    char szOrderBy[SQL_ORDER_BY_BUFFER_LEN];

    snprintf(
        szOrderBy, 
        SQL_ORDER_BY_BUFFER_LEN - 1, 
        " ORDER BY date DESC LIMIT %d;", 
        sqlRowLimit);

    strcat(szStatement, szOrderBy);

    result.retrieve(szStatement);

    log.exit("DBTransactionView::findTransactionsForAccountID()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransactionView::retrieveByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveReconciledByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectReconciledByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransactionView::retrieveReconciledByAccountIDForPeriod()");

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveNonRecurringByAccountIDForPeriod(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectNonRecurringByAccountIDBetweenDates, 
        accountId,
        firstDate.shortDate().c_str(),
        secondDate.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBTransactionView::retrieveNonRecurringByAccountIDForPeriod()");

    return result;
}
