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

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountID(pfm_id_t accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountID()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> result;

    result.retrieve(statement);

    log.exit("DBRecurringChargeView::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    DBCriteria criteria;
    criteria.add("account_id", DBCriteria::equal_to, accountId);
    criteria.add("date", DBCriteria::greater_than_or_equal, dateAfter.shortDate());
    criteria.add("date", DBCriteria::less_than, dateBefore.shortDate());

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringChargeView> result;

    result.retrieve(statement);

    log.exit("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    return result;
}
