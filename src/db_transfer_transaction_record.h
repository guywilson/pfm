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
    protected:
        struct Columns {
            static constexpr const char * transactionToId = "transaction_to_id";
            static constexpr const char * transactionFromId = "transaction_from_id";
            static constexpr const char * transactionDate = "transaction_date";
        };

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

        const string getTableName() const override {
            return "transfer_transaction_record";
        }

        const string getClassName() const override {
            return "DBTransferTransactionRecord";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::transactionToId, transactionToId.getValue()},
                {Columns::transactionFromId, transactionFromId.getValue()},
                {Columns::transactionDate, transactionDate.shortDate()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::transactionToId, transactionToId.getValue()},
                {Columns::transactionFromId, transactionFromId.getValue()},
                {Columns::transactionDate, transactionDate.shortDate()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::transactionToId) {
                transactionToId = column.getIDValue();
            }
            else if (column.getName() == Columns::transactionFromId) {
                transactionFromId = column.getIDValue();
            }
            else if (column.getName() == Columns::transactionDate) {
                transactionDate = column.getValue();
            }
        }

        int retrieveByTransactionToId(pfm_id_t & transactionToId);
        int retrieveByTransactionFromId(pfm_id_t & transactionFromId);

        static DBTransferTransactionRecord createFromTransactions(DBTransaction & transactionTo, DBTransaction & transactionFrom);
};

#endif
