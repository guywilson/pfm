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

int DBCarriedOver::retrieveLatestByAccountId(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBCarriedOver::retrieveLatestByAccountId()");

    DBCriteria criteria;
    criteria.add(Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(Columns::date, DBCriteria::descending);
    criteria.setRowLimit(1);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();

    DBResult<DBCarriedOver> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBCarriedOver::retrieveLatestByAccountId()");

    return rowsRetrievedCount;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveByAccountId(pfm_id_t & accountId) {
    Logger & log = Logger::getInstance();
    log.entry("DBCarriedOver::retrieveByAccountId()");

    DBCriteria criteria;
    criteria.add(Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.addOrderBy(Columns::date, DBCriteria::descending);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();

    DBResult<DBCarriedOver> result;

    result.retrieve(statement);

    log.exit("DBCarriedOver::retrieveByAccountId()");

    return result;
}

DBResult<DBCarriedOver> DBCarriedOver::retrieveByAccountIdAfterDate(pfm_id_t & accountId, StrDate & after) {
    Logger & log = Logger::getInstance();
    log.entry("DBCarriedOver::retrieveByAccountIdAfterDate()");

    DBCriteria criteria;
    criteria.add(Columns::accountId, DBCriteria::equal_to, accountId);
    criteria.add(Columns::date, DBCriteria::greater_than_or_equal, after);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();

    DBResult<DBCarriedOver> result;

    result.retrieve(statement);

    log.exit("DBCarriedOver::retrieveByAccountIdAfterDate()");

    return result;
}

DBCarriedOver DBCarriedOver::createForPeriod(pfm_id_t & accountId, Money & startingBalance, StrDate & startDate, StrDate & endDate) {
    Logger & log = Logger::getInstance();
    log.entry("DBCarriedOver::createForPeriod()");

    PFM_DB & db = PFM_DB::getInstance();
    
    db.begin();

    DBCarriedOver co;

    try {
        DBTransactionView tr;
        DBResult<DBTransactionView> transactionResult = tr.retrieveByAccountIDForPeriod(accountId, DBCriteria::ascending, startDate, endDate);

        Money total = startingBalance;

        for (int i = 0;i < transactionResult.size();i++) {
            DBTransaction transaction = transactionResult.at(i);
            total += transaction.getSignedAmount();
        }

        log.debug(
            "Creating carried over log with date '%s' from %d transactions for total %s", 
            endDate.shortDate().c_str(), 
            transactionResult.size(), 
            total.localeFormattedStringValue().c_str());

        co.balance = total;
        co.accountId = accountId;
        co.date = endDate;
        co.description = "Carried over (" + endDate.shortDate() + ")";

        co.save();
    }
    catch (pfm_error & e) {
        db.rollback();
        throw e;
    }

    db.commit();

    log.exit("DBCarriedOver::createForPeriod()");

    return co;
}
