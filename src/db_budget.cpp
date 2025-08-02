#include <string>

#include "logger.h"
#include "pfm_error.h"
#include "db_budget.h"
#include "db_budget_track.h"
#include "db_base.h"
#include "db_category.h"
#include "db_payee.h"
#include "db.h"
#include "strdate.h"

using namespace std;

DBResult<DBBudget> DBBudget::retrieveByPayeeCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudget> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByPayeeCode, 
        code.c_str());

    result.retrieve(szStatement);

    return result;
}

DBResult<DBBudget> DBBudget::retrieveByCategoryCode(string & code) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBBudget> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByCategoryCode, 
        code.c_str());

    result.retrieve(szStatement);

    return result;
}

DBResult<DBBudget> DBBudget::retrieveByCategoryOrPayeeCode(string & categoryCode, string & payeeCode) {
    DBResult<DBBudget> result = retrieveByCategoryCode(categoryCode);
    DBResult<DBBudget> resultByPayee = retrieveByPayeeCode(payeeCode);

    for (int i = 0;i < resultByPayee.size();i++) {
        DBBudget budgetByPayee = resultByPayee.at(i);
        result.addRow(budgetByPayee);
    }

    return result;
}

int DBBudget::createOutstandingTrackingRecords() {
    DBBudgetTrack budgetTrack;
    int hasLatestTrackingRecord = budgetTrack.retrieveLatestByBudgetId(this->id);

    StrDate today;
    StrDate latestDate;

    if (hasLatestTrackingRecord) {
        if (budgetTrack.date.month() < today.month()) {
            latestDate = budgetTrack.date;
        }
    }
    else {
        latestDate = startDate;
    }

    int numRecordsCreated = 0;

    for (int i = latestDate.month();i < today.month();i++) {
        DBBudgetTrack track;

        track.budgetId = this->id;

        latestDate.addMonths(1);
        track.date = latestDate;

        track.balance = 0.0;

        track.save();

        numRecordsCreated++;
    }

    return numRecordsCreated;
}

void DBBudget::afterInsert() {
    DBBudgetTrack track;

    track.date.set(startDate.year(), startDate.month(), 1);
    track.balance = 0.0;
    track.budgetId = id;

    track.save();
}
