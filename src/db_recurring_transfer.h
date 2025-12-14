#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "db_account.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_RECURRING_TRANSFER
#define __INCL_RECURRING_TRANSFER

class DBRecurringTransfer : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * recurringChargeId = "recurring_charge_id";
            static constexpr ColumnType recurringChargeId_type = ColumnType::ID;

            static constexpr const char * accountToId = "account_to_id";
            static constexpr ColumnType accountToId_type = ColumnType::ID;
        };

    public:
        DBAccount accountTo;
        
        pfm_id_t recurringChargeId;
        pfm_id_t accountToId;

        DBRecurringTransfer() : DBEntity() {
            clear();
        }

        DBRecurringTransfer(const DBRecurringTransfer & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->recurringChargeId.clear();
            this->accountToId.clear();
        }

        void set(const DBRecurringTransfer & src) {
            DBEntity::set(src);

            this->accountTo = src.accountTo;
            this->recurringChargeId = src.recurringChargeId;
            this->accountToId = src.accountToId;
        }

        void print() {
            DBEntity::print();

            cout << "RecurringChargeId: " << recurringChargeId.getValue() << endl;
            cout << "AccountToId: " << accountToId.getValue() << endl;
        }

        void onRowComplete(int sequence) override {
            if (!accountToId.isNull()) {
                accountTo.retrieve(accountToId);
            }
        }

        bool inline isNull() {
            return (id.isNull() && accountToId.isNull() && recurringChargeId.isNull());
        }

        const string getTableName() const override {
            return "recurring_transfer";
        }

        const string getClassName() const override {
            return "DBRecurringTransfer";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::accountToId, Columns::accountToId_type}, accountToId.getValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::accountToId, Columns::accountToId_type}, accountToId.getValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::recurringChargeId) {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == Columns::accountToId) {
                accountToId = column.getIDValue();
            }
        }

        int retrieveByRecurringChargeId(pfm_id_t & recurringChargeId);
};

#endif
