/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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


int DBTransferTransactionRecord::retrieveByTransactionToId(pfm_id_t & transactionToId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::retrieveByTransactionToId()");

    DBCriteria criteria;
    criteria.add(Columns::transactionToId, DBCriteria::equal_to, transactionToId);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransferTransactionRecord> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result.at(0));
    }

    log.exit("DBTransferTransactionRecord::retrieveByTransactionToId()");

    return rowsRetrievedCount;
}

int DBTransferTransactionRecord::retrieveByTransactionFromId(pfm_id_t & transactionFromId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::retrieveByTransactionFromId()");

    DBCriteria criteria;
    criteria.add(Columns::transactionFromId, DBCriteria::equal_to, transactionFromId);

    std::string statement = getSelectStatement() +  criteria.getStatementCriteria();
    DBResult<DBTransferTransactionRecord> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result[0]);
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
    catch (std::exception & e) {
        log.error(
            "Failed to create transfer record from transactions to:%s, from:%s", 
            transactionTo.id.c_str(), 
            transactionFrom.id.c_str());
            
        db.rollback();
    }

    log.exit("DBTransferTransactionRecord::createFromTransactions()");

    return record;
}

int DBTransferTransactionRecord::retrieveByTransactionIds(pfm_id_t & sourceTransactionId, pfm_id_t & targetTransactionId) {
    Logger & log = Logger::getInstance();
    log.entry("DBTransferTransactionRecord::retrieveByTransactionIds()");

    DBCriteria criteria;
    criteria.add(Columns::transactionFromId, DBCriteria::equal_to, sourceTransactionId);
    criteria.add(Columns::transactionToId, DBCriteria::equal_to, targetTransactionId);

    std::string statement = getSelectStatement() + criteria.getStatementCriteria();

    DBResult<DBTransferTransactionRecord> result;

    int rowsRetrievedCount = result.retrieve(statement);

    if (rowsRetrievedCount == 1) {
        set(result[0]);
    }

    log.exit("DBTransferTransactionRecord::retrieveByTransactionIds()");

    return rowsRetrievedCount;
}
