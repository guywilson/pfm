#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db_transfer_transaction_record.h"
#include "db_transaction.h"
#include "db_v_transaction.h"
#include "db.h"
#include "strdate.h"

using namespace std;

int DBTransferTransactionRecord::retrieveByTransactionToId(pfm_id_t transactionToId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::retrieveByTransactionToId()");

    DBCriteria criteria;
    criteria.add("transaction_to_id", DBCriteria::equal_to, transactionToId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransferTransactionRecord> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBTransferTransactionRecord::retrieveByTransactionToId()");

    return rowsRetrievedCount;
}

int DBTransferTransactionRecord::retrieveByTransactionFromId(pfm_id_t transactionFromId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::retrieveByTransactionFromId()");

    DBCriteria criteria;
    criteria.add("transaction_from_id", DBCriteria::equal_to, transactionFromId);

    string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransferTransactionRecord> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBTransferTransactionRecord::retrieveByTransactionFromId()");

    return rowsRetrievedCount;
}

DBTransferTransactionRecord DBTransferTransactionRecord::createFromTransactions(DBTransaction & transactionTo, DBTransaction & transactionFrom) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::createFromTransactions()");

    DBTransferTransactionRecord record;

    PFM_DB & db = PFM_DB::getInstance();

    try {
        db.begin();

        record.transactionToId = transactionTo.id;
        record.transactionFromId = transactionFrom.id;
        record.transactionDate = transactionFrom.date;
        record.save();

        db.commit();
    }
    catch (exception & e) {
        log.error(
            "Failed to create transfer record from transactions to:%s, from:%s", 
            transactionTo.id.c_str(), 
            transactionFrom.id.c_str());
            
        db.rollback();
    }

    log.exit("DBTransferTransactionRecord::createFromTransactions()");

    return record;
}
