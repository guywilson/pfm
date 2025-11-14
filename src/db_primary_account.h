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
    protected:
        struct Columns {
            static constexpr const char * code = "account_code";
        };

    public:
        string code;

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
            
            if (column.getName() == Columns::code) {
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
            vector<pair<string, string>> columnValuePairs = {
                {Columns::code, code}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::code, code}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        static string getPrimaryAccountCode();
        static void setPrimaryAccount(string & accountCode);
};

#endif
