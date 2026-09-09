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
#include "db_v_recurring_charge.h"
#include "db.h"
#include "strdate.h"


DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::descending);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> result;

    result.retrieve(statement);

    log.exit("DBRecurringChargeView::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountIDBetweenDates(pfm_id_t & accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBPayment::Columns::date, DBCriteria::greater_than_or_equal, dateAfter);
    criteria.add(DBPayment::Columns::date, DBCriteria::less_than, dateBefore);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> result;

    result.retrieve(statement);

    log.exit("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    return result;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::getChargesOutstandingThisPeriod(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::getChargesOutstandingThisPeriod()");

    StrDate today;
    StrDate periodStart = StrDate::getPeriodStartDate(today);
    StrDate periodEnd = StrDate::getPeriodEndDate(today);

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::ascending);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> charges;

    charges.retrieve(statement);

    DBResult<DBRecurringChargeView> results;

    for (size_t i = 0;i < charges.size();i++) {
        DBRecurringChargeView charge = charges[i];

        StrDate chargeDate = charge.getNextRecurringTransactionDate();

        bool isChargeOutstanding = (charge.isChargeDueThisPeriod() && chargeDate > today && chargeDate <= periodEnd);

        log.debug(
            "Charge '%s' is %outstanding in this period '%s' -> '%s'", 
            charge.description.c_str(),
            (isChargeOutstanding ? "" : "not "), 
            periodStart.shortDate().c_str(), 
            periodEnd.shortDate().c_str());

        if (isChargeOutstanding) {
            results.addRow(charge);
        }
    }

    log.exit("DBRecurringChargeView::getChargesOutstandingThisPeriod()");

    return results;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::getChargesPaidThisPeriod(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::getChargesPaidThisPeriod()");

    StrDate today;
    StrDate periodStart = StrDate::getPeriodStartDate(today);

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::greater_than_or_equal, periodStart);
    criteria.add(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::less_than_or_equal, today);
    criteria.addOrderBy(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::ascending);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> results;

    results.retrieve(statement);

    log.exit("DBRecurringChargeView::getChargesPaidThisPeriod()");

    return results;
}
