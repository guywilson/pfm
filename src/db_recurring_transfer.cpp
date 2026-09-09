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
#include "db.h"
#include "strdate.h"
#include "logger.h"
#include "db_recurring_transfer.h"


int DBRecurringTransfer::retrieveByRecurringChargeId(pfm_id_t & recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringTransfer::retrieveByRecurringChargeId");

    DBCriteria criteria;
    criteria.add(Columns::recurringChargeId, DBCriteria::equal_to, recurringChargeId);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringTransfer> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBRecurringTransfer::retrieveByRecurringChargeId");

    return rowsRetrievedCount;
}
