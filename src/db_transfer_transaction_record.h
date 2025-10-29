#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "db_transaction.h"
#include "strdate.h"
#include "money.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __INCL_TRANSFER_TRANSACTION_RECORD
#define __INCL_TRANSFER_TRANSACTION_RECORD

class DBTransferTransactionRecord : public DBEntity {
    private:
        const char * sqlSelectByTransactionToId = 
                        "SELECT id, " \
                        "transaction_to_id," \
                        "transaction_from_id," \
                        "transaction_date," \
                        "created," \
                        "updated " \
                        "FROM transfer_transaction_record " \
                        "WHERE transaction_to_id = %s;";

        const char * sqlSelectByTransactionFromId = 
                        "SELECT id, " \
                        "transaction_to_id," \
                        "transaction_from_id," \
                        "transaction_date," \
                        "created," \
                        "updated " \
                        "FROM transfer_transaction_record " \
                        "WHERE transaction_from_id = %s;";

        const char * sqlInsert = 
                        "INSERT INTO transfer_transaction_record (" \
                        "transaction_to_id," \
                        "transaction_from_id," \
                        "transaction_date," \
                        "created," \
                        "updated) " \
                        "VALUES (%s, %s, '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE transfer_transaction_record SET " \
                        "transaction_to_id = %s," \
                        "transaction_from_id = %s," \
                        "transaction_date = '%s'" \
                        "updated = '%s' " \
                        "WHERE id = %s;";

    public:
        pfm_id_t transactionToId;
        pfm_id_t transactionFromId;
        StrDate transactionDate;

        DBTransferTransactionRecord() : DBEntity() {
            clear();
        }

        DBTransferTransactionRecord(const DBTransferTransactionRecord & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->transactionFromId.clear();
            this->transactionToId.clear();
            this->transactionDate.clear();
        }

        void set(const DBTransferTransactionRecord & src) {
            DBEntity::set(src);

            this->transactionFromId = src.transactionFromId;
            this->transactionToId = src.transactionToId;
            this->transactionDate = src.transactionDate;
        }

        void print() {
            DBEntity::print();

            cout << "TransactionFromId: " << transactionFromId.getValue() << endl;
            cout << "TransactionToId: " << transactionToId.getValue() << endl;
            cout << "TransactionDate: " << transactionDate.shortDate() << endl;
        }

        const char * getTableName() override {
            return "transfer_transaction_record";
        }

        const char * getClassName() override {
            return "DBTransferTransactionRecord";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                transactionToId.c_str(),
                transactionFromId.c_str(),
                transactionDate.shortDate().c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                transactionToId.c_str(),
                transactionFromId.c_str(),
                transactionDate.shortDate().c_str(),
                now.c_str(),
                id.c_str());

            return szStatement;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "transaction_to_id") {
                transactionToId = column.getIDValue();
            }
            else if (column.getName() == "transaction_from_id") {
                transactionFromId = column.getIDValue();
            }
            else if (column.getName() == "transaction_date") {
                transactionDate = column.getValue();
            }
        }

        int retrieveByTransactionToId(pfm_id_t transactionToId);
        int retrieveByTransactionFromId(pfm_id_t transactionFromId);

        static DBTransferTransactionRecord createFromTransactions(DBTransaction & transactionTo, DBTransaction & transactionFrom);
};

#endif
