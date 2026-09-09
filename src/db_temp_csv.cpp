/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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
