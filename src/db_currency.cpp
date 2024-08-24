#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_currency.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBCurrency::retrieveByCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCurrency> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCode, 
        code.c_str());

    int rowsRetrievedCount = result.executeSelect(szStatement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.getResultAt(0));
}

DBResult<DBCurrency> DBCurrency::retrieveAll() {
    DBResult<DBCurrency> result;

    result.executeSelect(sqlSelectAll);

    return result;
}
