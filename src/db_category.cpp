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

    DBCriteria criteria;
    criteria.add(Columns::code, DBCriteria::equal_to, code);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBCategory> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }
    else {
        clear();
    }

    log.exit("DBCategory::retrieveByCode()");
}

DBResult<DBCategory> DBCategory::retrieveOrderedByCode() {
    Logger & log = Logger::getInstance();
    log.entry("DBCategory::retrieveOrderedByCode()");

    DBCriteria criteria;
    criteria.addOrderBy(Columns::code, DBCriteria::ascending);

    string statement = getSelectStatement() + criteria.getOrderBy();
    DBResult<DBCategory> result;

    result.retrieve(statement);

    log.exit("DBCategory::retrieveOrderedByCode()");

    return result;
}
