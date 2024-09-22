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
    DBResult<DBTransactionView> result = retrieveByStatementAndID(sqlSelectByAccountID, accountId);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransactionView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDSortedByDate, 
        accountId,
        (dateSortDirection == sort_ascending ? "ASC" : "DESC"));

    if (rowLimit > 0) {
        char szLimit[16];

        snprintf(szLimit, 16, " LIMIT %d;", rowLimit);
        strcat(szStatement, szLimit);
    }
    else {
        strcat(szStatement, ";");
    }

    result.retrieve(szStatement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::findTransactionsForAccountID(pfm_id_t accountId, string & criteria) {
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
    
    snprintf(
        &szStatement[strlen(szStatement)], 
        SQL_STATEMENT_BUFFER_LEN, 
        " ORDER BY date DESC LIMIT %d;", 
        sqlRowLimit);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBTransactionView> DBTransactionView::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
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

    return result;
}
