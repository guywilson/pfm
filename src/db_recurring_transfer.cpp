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

    DBCriteria criteria;
    criteria.add("recurring_charge_id", DBCriteria::equal_to, recurringChargeId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBRecurringTransfer> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBRecurringTransfer::retrieveByRecurringChargeId");

    return rowsRetrievedCount;
}
