#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_recurring_charge.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBRecurringCharge::retrieveByID(sqlite3_int64 id) {
    char                    szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                     rowsRetrievedCount;
    DBRecurringChargeResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByID, 
        id);

    PFM_DB & db = PFM_DB::getInstance();
    rowsRetrievedCount = db.executeSelect(szStatement, &result);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

void DBRecurringCharge::retrieveByAccountID(sqlite3_int64 accountId) {
    char                    szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                     rowsRetrievedCount;
    DBRecurringChargeResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    rowsRetrievedCount = db.executeSelect(szStatement, &result);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

DBRecurringChargeResult DBRecurringCharge::retrieveAll() {
    DBRecurringChargeResult result;

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect(sqlSelectAll, &result);

    return result;
}
