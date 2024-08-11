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
#include "db.h"
#include "strdate.h"

using namespace std;

// int PFM_DB::getRecurringTransactionsForNextPaymentDate(
//                 DBRecurringCharge & charge,
//                 DBTransactionResult * result)
// {
//     char *          pszErrorMsg;
//     char            szStatement[SQL_STATEMENT_BUFFER_LEN];
//     int             error;

//     const char * pszTemplate = 
//                 "SELECT " \
//                 "id " \
//                 "FROM account_transaction " \
//                 "WHERE account_id = %lld " \
//                 "AND recurring_charge_id = %lld " \
//                 "AND date = '%s';";

//     snprintf(
//         szStatement, 
//         SQL_STATEMENT_BUFFER_LEN, 
//         pszTemplate, 
//         charge.accountId, 
//         charge.id,
//         charge.nextPaymentDate.c_str());

//     error = sqlite3_exec(dbHandle, szStatement, transactionCallback, result, &pszErrorMsg);

//     if (error) {
//         throw pfm_error(
//                 pfm_error::buildMsg(
//                     "Failed to get transaction: %s", 
//                     pszErrorMsg), 
//                 __FILE__, 
//                 __LINE__);
//     }

//     // result->results[0].print();

//     return result->numRows;
// }

// /*
// ** 1) Get the latest transaction for the recurring charge.
// ** 2) If no transaction exists, create one for the RC start date
// ** 3) Else If the latest transaction.date < today, add to date as per frequency
// ** 4) Create the new transaction
// ** 5) Goto 1
// */
// int PFM_DB::getLatestTransactionForRecurringCharge(DBRecurringCharge & charge, DBTransactionResult * result) {
//     char *          pszErrorMsg;
//     char            szStatement[SQL_STATEMENT_BUFFER_LEN];
//     int             error;

//     const char * pszTemplate = 
//                 "SELECT " \
//                 "id " \
//                 "FROM account_transaction " \
//                 "WHERE account_id = %lld " \
//                 "AND recurring_charge_id = %lld " \
//                 "ORDER BY date DESC " \
//                 "LIMIT 1;";

//     snprintf(
//         szStatement, 
//         SQL_STATEMENT_BUFFER_LEN, 
//         pszTemplate, 
//         charge.accountId, 
//         charge.id,
//         charge.nextPaymentDate.c_str());

//     error = sqlite3_exec(dbHandle, szStatement, transactionCallback, result, &pszErrorMsg);

//     if (error) {
//         throw pfm_error(
//                 pfm_error::buildMsg(
//                     "Failed to get transaction: %s", 
//                     pszErrorMsg), 
//                 __FILE__, 
//                 __LINE__);
//     }

//     // result->results[0].print();

//     return result->numRows;
// }

// int PFM_DB::createDueRecurringTransactionsForAccount(sqlite3_int64 accountId) {
//     DBTransaction           transaction;
//     DBTransactionResult     trResult;
//     DBRecurringChargeResult rcResult;
//     int                     i;
//     int                     numRecurringCharges;
//     int                     numTransactionsCreated = 0;

//     numRecurringCharges = getRecurringChargesForAccount(accountId, &rcResult);

//     for (i = 0;i < numRecurringCharges;i++) {
//         DBRecurringCharge charge = rcResult.results[i];

//         /*
//         ** If the charge is due, but doesn't have a 
//         ** corresponding transaction yet, create it...
//         */
//         if (charge.isDue()) {
//             if (getRecurringTransactionsForNextPaymentDate(charge, &trResult) == 0) {
//                 transaction.setFromRecurringCharge(charge);

//                 createTransaction(transaction);

//                 numTransactionsCreated++;
//             }
//         }
//     }

//     return numTransactionsCreated;
// }

void DBTransaction::retrieveByID(sqlite3_int64 id) {
    char                szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                 rowsRetrievedCount;
    DBTransactionResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByID, 
        id);

    PFM_DB & db = PFM_DB::getInstance();
    rowsRetrievedCount = db.executeSelect(szStatement, &result);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg(
                    "Expected exactly 1 row, got %d", 
                    rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

DBTransactionResult DBTransaction::retrieveByAccountID(sqlite3_int64 accountId) {
    char                szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBTransactionResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect(szStatement, &result);

    return result;
}

DBTransactionResult DBTransaction::findTransactionsForAccountID(
                                            sqlite3_int64 accountId, 
                                            DBCriteria * criteria, 
                                            int numCriteria)
{
    char                szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                 sqlRowLimit = SQL_ROW_LIMIT;
    DBTransactionResult result;

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
    db.executeSelect(szStatement, &result);

    return result;
}
