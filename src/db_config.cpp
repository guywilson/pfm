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

void DBConfig::retrieveByKey(const string & key) {
    Logger & log = Logger::getInstance();
    log.entry("DBConfig::retrieveByKey()");

    DBCriteria criteria;
    criteria.add(Columns::key, DBCriteria::equal_to, key);
    criteria.add(Columns::isVisible, true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBConfig> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    set(result.at(0));

    log.exit("DBConfig::retrieveByKey()");
}

DBResult<DBConfig> DBConfig::retrieveAllVisible() {
    Logger & log = Logger::getInstance();
    log.entry("DBConfig>::retrieveAllVisible()");

    DBCriteria criteria;
    criteria.add(Columns::isVisible, true);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBConfig> result;

    result.retrieve(statement);

    log.exit("DBConfig>::retrieveAllVisible()");

    return result;
}
