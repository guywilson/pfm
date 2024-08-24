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

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBTransaction>> (szStatement, &result);

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

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBTransaction>> (szStatement, &result);

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

DBResult<DBTransaction> DBTransaction::findTransactionsForAccountID(
                                            pfm_id_t accountId, 
                                            DBCriteria * criteria, 
                                            int numCriteria)
{
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int sqlRowLimit = SQL_ROW_LIMIT;
    DBResult<DBTransaction> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    if (numCriteria > 0 && criteria != NULL) {
        /*
        ** Remove the trailing ';' from the statement...
        */
        szStatement[strlen(szStatement) - 1] = 0;

        for (int i = 0;i < numCriteria;i++) {
            strcat(szStatement, " AND ");
            strcat(szStatement, criteria[i].columnName.c_str());
            strcat(szStatement, " ");

            switch (criteria[i].operation) {
                case less_than:
                    strcat(szStatement, "< ");
                    break;

                case less_than_or_equal_to:
                    strcat(szStatement, "<= ");
                    break;

                case greater_than:
                    strcat(szStatement, "> ");
                    break;

                case greater_than_or_equal_to:
                    strcat(szStatement, ">= ");
                    break;

                case equals:
                    strcat(szStatement, "= ");
                    break;

                case not_equals:
                    strcat(szStatement, "<> ");
                    break;

                case like:
                    strcat(szStatement, "~ ");
                    break;

                case unknown:
                    break;
            }

            switch (criteria[i].columnType) {
                case db_column_type::date:
                    strcat(szStatement, "'");
                    strcat(szStatement, criteria[i].value.c_str());
                    strcat(szStatement, "'");
                    break;

                case db_column_type::text:
                    strcat(szStatement, "'");
                    strcat(szStatement, criteria[i].value.c_str());
                    strcat(szStatement, "'");
                    break;

                case db_column_type::numeric:
                    strcat(szStatement, criteria[i].value.c_str());
                    break;
            }
        }
    }

    snprintf(
        &szStatement[strlen(szStatement)], 
        SQL_STATEMENT_BUFFER_LEN, 
        " ORDER BY date DESC LIMIT %d;", 
        sqlRowLimit);

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBTransaction>> (szStatement, &result);

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

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBTransaction>> (szStatement, &result);

    return result;
}

void DBTransaction::afterInsert() {
    DBAccount account;
    account.id = accountId;

    DBResult<DBAccount> result;
    account.retrieveByID(&result);

    account.currentBalance += this->getSignedAmount();
    account.save();
}

void DBTransaction::beforeUpdate() {
    DBTransaction transaction;
    transaction.id = this->id;

    DBResult<DBTransaction> result;
    transaction.retrieveByID(&result);

    /*
    ** If the amount has been updated on the transaction,
    ** revert and re-add the transaction amount to the 
    ** current balance...
    */
    if (this->amount != transaction.amount) {
        DBAccount account;
        account.id = transaction.accountId;

        DBResult<DBAccount> result;
        account.retrieveByID(&result);

        account.currentBalance -= transaction.getSignedAmount();
        account.currentBalance += this->getSignedAmount();

        account.save();
    }
}

void DBTransaction::afterRemove() {
    DBAccount account;
    account.id = accountId;

    DBResult<DBAccount> result;
    account.retrieveByID(&result);

    account.currentBalance -= getSignedAmount();

    account.save();
}
