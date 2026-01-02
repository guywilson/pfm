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

using namespace std;

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountID(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
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

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> result;

    result.retrieve(statement);

    log.exit("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    return result;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::getChargesOutstandingThisPeriod(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::getChargesOutstandingThisPeriod()");

    StrDate today;
    StrDate periodStart = getPeriodStartDate(today);
    StrDate periodEnd = getPeriodEndDate(today);

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::ascending);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> charges;

    charges.retrieve(statement);

    DBResult<DBRecurringChargeView> results;

    for (int i = 0;i < charges.size();i++) {
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
    StrDate periodStart = getPeriodStartDate(today);

    DBCriteria criteria;
    criteria.add(DBPayment::Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::greater_than_or_equal, periodStart);
    criteria.add(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::less_than_or_equal, today);
    criteria.addOrderBy(DBRecurringCharge::Columns::lastPaymentDate, DBCriteria::ascending);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> results;

    results.retrieve(statement);

    log.exit("DBRecurringChargeView::getChargesPaidThisPeriod()");

    return results;
}
