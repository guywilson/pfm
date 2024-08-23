#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "money.h"

using namespace std;

#ifndef __INCL_CARRIED_OVER
#define __INCL_CARRIED_OVER

class DBCarriedOverResult;

class DBCarriedOver : public DBEntity {
    private:
        const char * sqlSelectByID = 
                        "SELECT id, " \
                        "account_id," \
                        "date," \
                        "description," \
                        "balance," \
                        "created," \
                        "updated " \
                        "FROM carried_over_log " \
                        "WHERE id = %lld;";

        const char * sqlSelectByAccountId = 
                        "SELECT id, " \
                        "account_id," \
                        "date," \
                        "description," \
                        "balance," \
                        "created," \
                        "updated " \
                        "FROM carried_over_log " \
                        "WHERE account_id = %lld;";

        const char * sqlSelectLatestByAccountId = 
                        "SELECT id, " \
                        "account_id," \
                        "date," \
                        "description," \
                        "balance," \
                        "created," \
                        "updated " \
                        "FROM carried_over_log " \
                        "WHERE account_id = %lld " \
                        "ORDER BY date DESC " \
                        "LIMIT 1;";

        const char * sqlSelectAll = 
                        "SELECT id, " \
                        "account_id," \
                        "date," \
                        "description," \
                        "balance," \
                        "created," \
                        "updated " \
                        "FROM carried_over_log;";

        const char * sqlInsert = 
                        "INSERT INTO carried_over_log (" \
                        "account_id," \
                        "date," \
                        "description," \
                        "balance," \
                        "created," \
                        "updated) " \
                        "VALUES (%lld, '%s', '%s', %s, '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE carried_over_log SET " \
                        "account_id = %lld," \
                        "date = '%s'," \
                        "description = '%s'," \
                        "balance = %s," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM carried_over_log WHERE id = %lld;";

    public:
        pfm_id_t                accountId;
        StrDate                 date;
        string                  description;
        Money                   balance;

        DBCarriedOver() : DBEntity() {
            clear();
        }

        DBCarriedOver(const DBCarriedOver & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->accountId = 0;
            this->date.clear();
            this->description = "";
            this->balance = 0.0;
        }

        void set(const DBCarriedOver & src) {
            DBEntity::set(src);

            this->accountId = src.accountId;
            this->date = src.date;
            this->description = src.description;
            this->balance =  src.balance;
        }

        void print() {
            DBEntity::print();

            cout << "AccountId: " << accountId << endl;
            cout << "Date: '" << date.shortDate() << "'" << endl;
            cout << "Description: '" << description << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Balance: " << balance.getFormattedStringValue() << endl;
        }

        const char * getTableName() override {
            return "carried_over_log";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                accountId,
                date.shortDate().c_str(),
                description.c_str(),
                balance.getRawStringValue().c_str(),
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
                accountId,
                date.shortDate().c_str(),
                description.c_str(),
                balance.getRawStringValue().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void retrieveByID(pfm_id_t id);
        int retrieveLatestByAccountId(pfm_id_t accountId);
        DBCarriedOverResult retrieveByAccountId(pfm_id_t accountId);
        DBCarriedOverResult retrieveAll();
};

class DBCarriedOverResult : public DBResult {
    private:
        vector<DBCarriedOver>       results;

    public:
        DBCarriedOverResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBCarriedOver getResultAt(int i) {
            if (getNumRows() > i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "getResultAt(): Index out of range: numRows: %d, requested row: %d", getNumRows(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        void processRow(DBRow & row) {
            DBCarriedOver carriedOver;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    carriedOver.id = column.getIDValue();
                }
                else if (column.getName() == "account_id") {
                    carriedOver.accountId = column.getIDValue();
                }
                else if (column.getName() == "date") {
                    carriedOver.date = column.getValue();
                }
                else if (column.getName() == "description") {
                    carriedOver.description = column.getValue();
                }
                else if (column.getName() == "balance") {
                    carriedOver.balance = column.getValue();
                }
                else if (column.getName() == "created") {
                    carriedOver.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    carriedOver.updatedDate = column.getValue();
                }
            }
            
            results.push_back(carriedOver);
            incrementNumRows();
        }
};

#endif
