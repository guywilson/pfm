#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_payee.h"
#include "db.h"
#include "strdate.h"

using namespace std;

void DBPayee::retrieveByCode(string & code) {
    Logger & log = Logger::getInstance();
    log.entry("DBPayee::retrieveByCode()");

    DBCriteria criteria;
    criteria.add("code", DBCriteria::equal_to, code);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBPayee> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }
    else {
        clear();
    }

    log.exit("DBPayee::retrieveByCode()");
}
