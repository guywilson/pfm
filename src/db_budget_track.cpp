#include <string>

#include "logger.h"
#include "pfm_error.h"
#include "db_budget_track.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBBudgetTrack> DBBudgetTrack::retrieveByBudgetId(pfm_id_t budgetId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudgetTrack> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByBudgetId, 
        budgetId);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBBudgetTrack> DBBudgetTrack::retrieveByBudgetIdAfterDate(pfm_id_t budgetId, StrDate & date) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudgetTrack> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByBudgetIdAfterDate, 
        budgetId,
        date.shortDate().c_str());

    result.retrieve(szStatement);

    return result;
}

int DBBudgetTrack::retrieveLatestByBudgetId(pfm_id_t budgetId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudgetTrack> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectLatestByBudgetId, 
        budgetId);

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    return rowsRetrievedCount;
}
