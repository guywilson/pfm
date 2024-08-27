#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_recurring_charge_view.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBRecurringChargeView> DBRecurringChargeView::retrieveByAccountID(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringChargeView> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountID, 
        accountId);

    result.retrieve(szStatement);

    return result;
}
