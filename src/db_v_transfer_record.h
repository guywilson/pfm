#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "db_transaction.h"
#include "strdate.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_TRANSFER_RECORD_VIEW
#define __INCL_TRANSFER_RECORD_VIEW

class DBTransferRecordView : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * transactionFromID = "transaction_from_id";
            static constexpr ColumnType transactionFromID_type = ColumnType::ID;

            static constexpr const char * transactionToID = "transaction_to_id";
            static constexpr ColumnType transactionToID_type = ColumnType::ID;

            static constexpr const char * accountFrom = "account_from";
            static constexpr ColumnType accountFrom_type = ColumnType::TEXT;

            static constexpr const char * accountTo = "account_to";
            static constexpr ColumnType accountTo_type = ColumnType::TEXT;

            static constexpr const char * categoryCode = "category_code";
            static constexpr ColumnType categoryCode_type = ColumnType::TEXT;

            static constexpr const char * payeeCode = "payee_code";
            static constexpr ColumnType payeeCode_type = ColumnType::TEXT;

            static constexpr const char * date = "date";
            static constexpr ColumnType date_type = ColumnType::DATE;

            static constexpr const char * description = "description";
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * amount = "amount";
            static constexpr ColumnType amount_type = ColumnType::MONEY;
        };

    public:
        pfm_id_t transactionFromID;
        pfm_id_t transactionToID;
        string accountFromCode;
        string accountToCode;
        string payeeCode;
        string categoryCode;
        StrDate date;
        string description;
        Money amount;

        DBTransferRecordView() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->transactionFromID.clear();
            this->transactionToID.clear();
            this->accountFromCode.clear();
            this->accountToCode.clear();
            this->payeeCode.clear();
            this->categoryCode.clear();
            this->date.clear();
            this->description.clear();
            this->amount = 0.0;
        }

        void set(const DBTransferRecordView & src) {
            DBEntity::set(src);

            this->transactionFromID = src.transactionFromID;
            this->transactionToID = src.transactionToID;
            this->accountFromCode = src.accountFromCode;
            this->accountToCode = src.accountToCode;
            this->payeeCode = src.payeeCode;
            this->categoryCode = src.categoryCode;
            this->date = src.date;
            this->description = src.description;
            this->amount = src.amount;
        }

        void print() {
            DBEntity::print();

            cout << "AccountFromCode: '" << accountFromCode << "'" << endl;
            cout << "AccountToCode: '" << accountToCode << "'" << endl;
            cout << "PayeeCode: '" << payeeCode << "'" << endl;
            cout << "CategoryCode: '" << categoryCode << "'" << endl;
            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "Amount: '" << amount.rawStringValue() << "'" << endl;
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::accountFrom) {
                accountFromCode = column.getValue();
            }
            else if (column.getName() == Columns::accountTo) {
                accountToCode = column.getValue();
            }
            else if (column.getName() == Columns::transactionFromID) {
                transactionFromID = column.getIDValue();
            }
            else if (column.getName() == Columns::transactionToID) {
                transactionToID = column.getIDValue();
            }
            else if (column.getName() == Columns::payeeCode) {
                payeeCode = column.getValue();
            }
            else if (column.getName() == Columns::categoryCode) {
                categoryCode = column.getValue();
            }
            else if (column.getName() == Columns::date) {
                date = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::amount) {
                amount = column.doubleValue();
            }
        }

        const string getTableName() const override {
            return "v_transfer_record";
        }

        const string getClassName() const override {
            return "DBTransferRecordView";
        }
};

#endif
