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

void DBCarriedOver::retrieveByID(pfm_id_t id) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    int rowsRetrievedCount;
    DBCarriedOverResult result;

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

void DBCarriedOver::retrieveLatestByAccountId(pfm_id_t accountId) {
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

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
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
