#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_category.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBCategory::retrieveByCode(string & code) {
    Logger & log = Logger::getInstance();
    log.entry("DBCategory::retrieveByCode()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCategory> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCode, 
        code.c_str());

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }
    else {
        clear();
    }

    log.exit("DBCategory::retrieveByCode()");
}
