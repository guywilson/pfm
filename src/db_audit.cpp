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

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "strdate.h"
#include "logger.h"
#include "pfm_error.h"
#include "db.h"
#include "db_base.h"
#include "db_audit.h"

int auditOnWriteHandler(const std::string & operation, const std::string & entityName, const std::string & statement) {
    static DBAuditInteraction audit;

    /*
    ** We must not record inserts into the audit_interaction table...
    */
    if (entityName != audit.getTableName()) {
        audit.auditTimestamp = StrDate::getTimestamp();
        audit.entityName = entityName;
        audit.sqlOperation = operation;
        audit.sqlStatement = statement;

        audit.save();
    }

    audit.clear();

    return 0;
}

DBResult<DBAuditInteraction> DBAuditInteraction::retrieveByDateRange(const StrDate & startDate, const StrDate & endDate) {
    Logger & log = Logger::getInstance();

    log.entry("DBAuditInteraction::retrieveByDateRange()");

    DBCriteria criteria;
    criteria.add(Columns::auditTimestamp, DBCriteria::sql_operator::greater_than_or_equal, startDate.shortDate() + " 00.00.00");
    criteria.add(Columns::auditTimestamp, DBCriteria::sql_operator::less_than_or_equal, endDate.shortDate() + " 23.59.59");
    criteria.addOrderBy(Columns::auditTimestamp, DBCriteria::sql_order::descending);

    std::string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBAuditInteraction> results;
    results.retrieve(statement);

    log.exit("DBAuditInteraction::retrieveByDateRange()");

    return results;
}

void DBAuditInteraction::deleteBeforeDate(const StrDate & date) {
    PFM_DB & db = PFM_DB::getInstance();
    Logger & log = Logger::getInstance();

    log.entry("DBAuditInteraction::deleteBeforeDate()");

    DBCriteria criteria;
    criteria.add(Columns::auditTimestamp, DBCriteria::sql_operator::less_than, date.shortDate() + " 00:00:00");

    std::string statement = getDeleteStatement() + criteria.getStatementCriteria();

    db.executeDelete(statement);

    log.exit("DBAuditInteraction::deleteBeforeDate()");
}
