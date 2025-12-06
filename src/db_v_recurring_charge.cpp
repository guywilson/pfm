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

DBResult<DBRecurringChargeView> DBRecurringChargeView::getOutstandingChargesDueThisPeriod(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::getOutstandingChargesDueThisPeriod()");

    StrDate today;
    StrDate periodStart = getPeriodStartDate(today);
    StrDate periodEnd = getPeriodEndDate(today);

    DBRecurringChargeView c;
    DBResult<DBRecurringChargeView> charges = c.retrieveByAccountID(accountId);

    DBResult<DBRecurringChargeView> results;

    for (int i = 0;i < charges.size();i++) {
        DBRecurringChargeView charge = charges[i];

        bool isChargeUnpaid = (!charge.lastPaymentDate.isNull() ? (charge.lastPaymentDate < periodStart) : true);

        log.debug(
            "Charge '%s' is %sunpaid in this period '%s' -> '%s'", 
            charge.description.c_str(),
            (isChargeUnpaid ? "" : "not "), 
            periodStart.shortDate().c_str(), 
            periodEnd.shortDate().c_str());

        if (isChargeUnpaid && charge.isChargeDueThisPeriod()) {
            log.debug("Charge '%s' is outstanding...", charge.description.c_str());
            results.addRow(charge);
        }
    }

    log.exit("DBRecurringChargeView::getOutstandingChargesDueThisPeriod()");

    return results;
}
