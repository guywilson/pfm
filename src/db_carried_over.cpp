#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_carried_over.h"
#include "db.h"
#include "strdate.h"

using namespace std;

int DBCarriedOver::retrieveLatestByAccountId(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int rowsRetrievedCount;
    DBCarriedOverResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectLatestByAccountId, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    rowsRetrievedCount = db.executeSelect(szStatement, &result);

    if (rowsRetrievedCount == 1) {
        set(result.getResultAt(0));
    }

    return rowsRetrievedCount;
}

DBCarriedOverResult DBCarriedOver::retrieveByAccountId(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBCarriedOverResult result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountId, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect(sqlSelectAll, &result);

    return result;
}

DBCarriedOverResult DBCarriedOver::retrieveAll() {
    DBCarriedOverResult result;

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect(sqlSelectAll, &result);

    return result;
}
