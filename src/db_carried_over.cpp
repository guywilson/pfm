#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_carried_over.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "db.h"
#include "strdate.h"

using namespace std;

int DBCarriedOver::retrieveLatestByAccountId(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCarriedOver> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectLatestByAccountId, 
        accountId);

    int rowsRetrievedCount = result.retrieve(szStatement);

    if (rowsRetrievedCount == 1) {
        set(result.getResultAt(0));
    }

    return rowsRetrievedCount;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveByAccountId(pfm_id_t accountId) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCarriedOver> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountId, 
        accountId);

    result.retrieve(szStatement);

    return result;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveByAccountIdAfterDate(pfm_id_t accountId, StrDate & after) {
    char szStatement[SQL_STATEMENT_BUFFER_LEN];
    DBResult<DBCarriedOver> result;

    snprintf(
        szStatement, 
        SQL_STATEMENT_BUFFER_LEN, 
        sqlSelectByAccountIdAfterDate, 
        accountId,
        after.shortDate().c_str());

    result.retrieve(szStatement);

    return result;
}

void DBCarriedOver::createForPeriod(pfm_id_t accountId, Money & startingBalance, StrDate & startDate, StrDate & endDate) {
    DBTransactionView tr;
    DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(accountId, startDate, endDate);

    Money total = startingBalance;

    for (int i = 0;i < transactionResult.getNumRows();i++) {
        DBTransaction transaction = transactionResult.getResultAt(i);
        total += transaction.getSignedAmount();
    }

    this->balance = total;
    this->accountId = accountId;
    this->date = endDate;
    this->description = "Carried over (" + this->date.shortDate() + ")";

    save();
}
