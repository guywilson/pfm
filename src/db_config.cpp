#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_config.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBConfig::retrieveByKey(string & key) {
    retrieveByKey(key.c_str());
}

void DBConfig::retrieveByKey(const char * key) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBConfig> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByKey, 
        key);

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.at(0));
}
