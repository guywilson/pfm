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
    private:
        const char * sqlSelectByRecurringChargeId = 
                        "SELECT id, " \
                        "recurring_charge_id," \
                        "account_to_id," \
                        "created," \
                        "updated " \
                        "FROM recurring_transfer " \
                        "WHERE recurring_charge_id = %s;";

        const char * sqlInsert = 
                        "INSERT INTO recurring_transfer (" \
                        "recurring_charge_id," \
                        "account_to_id," \
                        "created," \
                        "updated) " \
                        "VALUES (%s, %s, '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE recurring_transfer SET " \
                        "recurring_charge_id = %s," \
                        "account_to_id = %s," \
                        "updated = '%s' " \
                        "WHERE id = %s;";

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

            this->recurringChargeId = src.recurringChargeId;
            this->accountToId = src.accountToId;
        }

        void print() {
            DBEntity::print();

            cout << "RecurringChargeId: " << recurringChargeId.getValue() << endl;
            cout << "AccountToId: " << accountToId.getValue() << endl;
        }

        bool inline isNull() {
            return (id.isNull() && accountToId.isNull() && recurringChargeId.isNull());
        }

        const char * getTableName() override {
            return "recurring_transfer";
        }

        const char * getClassName() override {
            return "DBRecurringTransfer";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                recurringChargeId.c_str(),
                accountToId.c_str(),
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
                recurringChargeId.c_str(),
                accountToId.c_str(),
                now.c_str(),
                id.c_str());

            return szStatement;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "recurring_charge_id") {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == "account_to_id") {
                accountToId = column.getIDValue();
            }
        }

        int retrieveByRecurringChargeId(pfm_id_t recurringChargeId);
};

#endif
