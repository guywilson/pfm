#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_PRIMARY_ACCOUNT
#define __INCL_PRIMARY_ACCOUNT

class DBPrimaryAccount : public DBEntity {
    private:
        const char * sqlInsert = 
                        "INSERT INTO primary_account (" \
                        "account_code," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE primary_account SET " \
                        "account_code = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %s;";

    public:
        string                  code;

        DBPrimaryAccount() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->code = "";
        }

        void set(const DBPrimaryAccount & src) {
            DBEntity::set(src);

            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", code);

            return r;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "account_code") {
                code = column.getValue();
            }
        }

        void print() {
            DBEntity::print();

            cout << "Code: '" << code << "'" << endl;
        }

        const char * getTableName() override {
            return "primary_account";
        }

        const char * getClassName() override {
            return "DBPrimaryAccount";
        }

        const string getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                code.c_str(),
                now.c_str(),
                now.c_str());

            return string(szStatement);
        }

        const string getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                now.c_str(),
                id.c_str());

            return string(szStatement);
        }

        static string getPrimaryAccountCode();
        static void setPrimaryAccount(string & accountCode);
};

#endif
