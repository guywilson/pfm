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
    DBResult<DBCarriedOver> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectLatestByAccountId, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    int rowsRetrievedCount = db.executeSelect <DBResult<DBCarriedOver>> (szStatement, &result);

    if (rowsRetrievedCount == 1) {
        set(result.getResultAt(0));
    }

    return rowsRetrievedCount;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveByAccountId(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCarriedOver> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountId, 
        accountId);

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBCarriedOver>> (sqlSelectAll, &result);

    return result;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveAll() {
    DBResult<DBCarriedOver> result;

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect <DBResult<DBCarriedOver>> (sqlSelectAll, &result);

    return result;
}
