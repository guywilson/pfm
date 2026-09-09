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


void DBCurrency::retrieveByCode(std::string & code) {
    Logger & log = Logger::getInstance();
    log.entry("DBCurrency::retrieveByCode()");

    DBCriteria criteria;
    criteria.add(Columns::code, DBCriteria::equal_to, code);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
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
