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

using namespace std;

int DBRecurringTransfer::retrieveByRecurringChargeId(pfm_id_t recurringChargeId) {
    Logger & log = Logger::getInstance();
    log.entry("DBRecurringTransfer::retrieveByRecurringChargeId");

    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBRecurringTransfer> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByRecurringChargeId, 
        recurringChargeId);

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBRecurringTransfer::retrieveByRecurringChargeId");

    return rowsRetrievedCount;
}
