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

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringChargeView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId.c_str());

    result.retrieve(szStatement);

    log.exit("DBRecurringChargeView::retrieveByAccountID()");

    return result;
}

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountIDBetweenDates(pfm_id_t accountId, StrDate & dateAfter, StrDate & dateBefore) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringChargeView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIDBetweenDates, 
        accountId.c_str(),
        dateAfter.shortDate().c_str(),
        dateBefore.shortDate().c_str());

    result.retrieve(szStatement);

    log.exit("DBRecurringChargeView::retrieveByAccountIDBetweenDates()");

    return result;
}
