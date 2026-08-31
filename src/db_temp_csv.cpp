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

DBResult<DBTempCSV> DBTempCSV::retrieveByDateRangeAndAmountForAccount(const std::string & accountCode, const StrDate & lower, const StrDate & upper, const Money & amount) {
    Logger & log = Logger::getInstance();
    log.entry("DBTempCSV::retrieveByDateRangeAndAmount()");

    DBCriteria criteria;

    criteria.add(Columns::accountCode, DBCriteria::sql_operator::equal_to, accountCode);
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

std::pair<StrDate, StrDate> DBTempCSV::getDateRangeForAccount(const std::string & accountCode) {
    Logger & log = Logger::getInstance();
    log.entry("DBTempCSV::getDateRangeForAccount()");

    DBCriteria criteria;

    criteria.add(Columns::accountCode, DBCriteria::sql_operator::equal_to, accountCode);
    criteria.addOrderBy(Columns::date, DBCriteria::sql_order::descending);

    std::string statement = "SELECT MIN(date) AS start_date, MAX(date) AS end_date FROM " + getTableName() + " WHERE account_code = '" + accountCode + "';";

    DBResult<DBTempCSV> result;
    result.retrieve(statement);

    log.info("Start date : '%s', end date : '%s'", result[0].startDate.shortDate().c_str(), result[0].endDate.shortDate().c_str());
    
    std::pair<StrDate, StrDate> dateRange(result[0].startDate, result[0].endDate);

    log.exit("DBTempCSV::getDateRangeForAccount()");

    return dateRange;
}
