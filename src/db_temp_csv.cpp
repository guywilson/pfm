#include <string>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "jfile.h"
#include "money.h"
#include "logger.h"
#include "db_temp_csv.h"

DBResult<DBTempCSV> DBTempCSV::retrieveByDateRangeAndAmount(const StrDate & lower, const StrDate & upper, const Money & amount) {
    Logger & log = Logger::getInstance();
    log.entry("DBTempCSV::retrieveByDateRangeAndAmount()");

    DBCriteria criteria;

    criteria.add(Columns::date, DBCriteria::sql_operator::greater_than_or_equal, lower);
    criteria.add(Columns::date, DBCriteria::sql_operator::less_than_or_equal, upper);
    criteria.add(Columns::amount, DBCriteria::sql_operator::equal_to, amount);
    criteria.addOrderBy(Columns::date, DBCriteria::sql_order::descending);

    std::string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBTempCSV> result;
    result.retrieve(statement);

    log.exit("DBTempCSV::retrieveByDateRangeAndAmount()");

    return result;
}
