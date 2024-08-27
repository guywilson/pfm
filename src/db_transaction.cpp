#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_category.h"
#include "db_payee.h"
#include "db_transaction.h"
#include "db_account.h"
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
    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectByAccountID, accountId);

    return result;
}

DBResult<DBTransaction> DBTransaction::retrieveByAccountID(pfm_id_t accountId, db_sort_t dateSortDirection, int rowLimit) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBTransaction> result;

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

int DBTransaction::findLatestByRecurringChargeID(pfm_id_t chargeId) {
    DBResult<DBTransaction> result = retrieveByStatementAndID(sqlSelectLatestByChargeID, chargeId);

    if (result.getNumRows() == 1) {
        set(result.getResultAt(0));
    }
    else if (result.getNumRows() > 1) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Expected no more than 1 row, got %d", 
                    result.getNumRows()), 
                __FILE__, 
                __LINE__);
    }

    return result.getNumRows();
}

DBResult<DBTransaction> DBTransaction::findTransactionsForAccountID(pfm_id_t accountId, string & criteria) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int sqlRowLimit = SQL_ROW_LIMIT;
    DBResult<DBTransaction> result;

    validateCriteria(criteria);
    
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

DBResult<DBTransaction> DBTransaction::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & firstDate, StrDate & secondDate) {
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

    return result;
}

void DBTransaction::afterInsert() {
    DBAccount account;
    account.retrieve(accountId);

    account.currentBalance += this->getSignedAmount();
    account.save();
}

void DBTransaction::beforeUpdate() {
    DBTransaction transaction;
    transaction.retrieve(id);

    /*
    ** If the amount has been updated on the transaction,
    ** revert and re-add the transaction amount to the 
    ** current balance...
    */
    if (this->amount != transaction.amount) {
        DBAccount account;
        account.retrieve(accountId);

        account.currentBalance -= transaction.getSignedAmount();
        account.currentBalance += this->getSignedAmount();

        account.save();
    }
}

void DBTransaction::afterRemove() {
    DBAccount account;
    account.retrieve(accountId);
    account.currentBalance -= getSignedAmount();
    account.save();


}
