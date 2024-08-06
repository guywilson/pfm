#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_payee.h"
#include "db.h"
#include "strdate.h"

using namespace std;

// void PFM_DB::translatePayeeCriteria(DBCriteria * c) {
//     DBPayeeResult   r;
//     char            szID[32];

//     getPayee(c->value, &r);

//     if (r.numRows) {
//         snprintf(szID, 32, "%lld", r.results[0].id);

//         c->columnName = "payee_id";
//         c->value = szID;
//         c->columnType = db_column_type::numeric;
//     }
//     else {
//         throw pfm_error(
//             pfm_error::buildMsg(
//                 "Payee '%s' does not exist", 
//                 c->value.c_str()), 
//             __FILE__, 
//             __LINE__);
//     }
// }

void DBPayee::retrieveByID(sqlite3_int64 id) {
    char                szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                 rowsRetrievedCount;
    DBPayeeResult       result;

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

void DBPayee::retrieveByCode(string & code) {
    char                szStatement[SQL_STATEMENT_BUFFER_LEN];
    int                 rowsRetrievedCount;
    DBPayeeResult       result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCode, 
        code.c_str());

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

DBPayeeResult DBPayee::retrieveAll() {
    DBPayeeResult result;

    PFM_DB & db = PFM_DB::getInstance();
    db.executeSelect(sqlSelectAll, &result);

    return result;
}
