#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_currency.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBCurrency::retrieveByCode(string & code) {
    Logger & log = Logger::getInstance();
    log.entry("DBCurrency::retrieveByCode()");

    DBCriteria criteria;
    criteria.add("code", DBCriteria::equal_to, code);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBCurrency> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.at(0));

    log.exit("DBCurrency::retrieveByCode()");
}
