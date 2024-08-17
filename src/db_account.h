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

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccountResult;

class DBAccount : public DBEntity {
    private:
        const char * sqlSelectByID = 
                        "SELECT id, " \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated " \
                        "FROM account " \
                        "WHERE id = %lld;";

        const char * sqlSelectByCode = 
                        "SELECT id," \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated " \
                        "FROM account " \
                        "WHERE code = '%s';";

        const char * sqlSelectAll = 
                        "SELECT id," \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance " \
                        "FROM account;";

        const char * sqlInsert = 
                        "INSERT INTO account (" \
                        "name," \
                        "code," \
                        "opening_balance," \
                        "current_balance," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', %s, %s, '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE account SET " \
                        "code = '%s'," \
                        "name = '%s'," \
                        "opening_balance = %s," \
                        "current_balance = %s," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM account WHERE id = %lld;";

    public:
        string                  name;
        string                  code;
        Money                   openingBalance;
        Money                   currentBalance;

        DBAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
            this->openingBalance = 0.0;
            this->currentBalance = 0.0;
        }

        void set(const DBAccount & src) {
            DBEntity::set(src);

            this->name =            src.name;
            this->code =            src.code;
            this->openingBalance =  src.openingBalance;
            this->currentBalance =  src.currentBalance;
        }

        void print() {
            DBEntity::print();

            cout << "Name: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Opening balance: " << openingBalance.getFormattedStringValue() << endl;
            cout << "Current balance: " << currentBalance.getFormattedStringValue() << endl;
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                name.c_str(),
                code.c_str(),
                openingBalance.getRawStringValue().c_str(),
                currentBalance.getRawStringValue().c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                name.c_str(),
                openingBalance.getRawStringValue().c_str(),
                currentBalance.getRawStringValue().c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void            retrieveByID(sqlite3_int64 id);
        void            retrieveByCode(string & code);
        DBAccountResult retrieveAll();
};

class DBAccountResult : public DBResult {
    private:
        vector<DBAccount>       results;

    public:
        DBAccountResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBAccount getResultAt(int i) {
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
            DBAccount account;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    account.id = column.getIDValue();
                }
                else if (column.getName() == "name") {
                    account.name = column.getValue();
                }
                else if (column.getName() == "code") {
                    account.code = column.getValue();
                }
                else if (column.getName() == "opening_balance") {
                    account.openingBalance = column.getDoubleValue();
                }
                else if (column.getName() == "current_balance") {
                    account.currentBalance = column.getDoubleValue();
                }
                else if (column.getName() == "created") {
                    account.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    account.updatedDate = column.getValue();
                }
            }
            
            results.push_back(account);
            incrementNumRows();
        }
};

#endif
