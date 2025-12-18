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
            static constexpr ColumnType code_type = ColumnType::TEXT;
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

        void backup(ofstream & os) override {
            DBResult<DBPrimaryAccount> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << endl;

            for (int i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << endl;
            }

            os.flush();
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

        const string getTableName() const override {
            return "primary_account";
        }

        const string getClassName() const override {
            return "DBPrimaryAccount";
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::code, Columns::code_type}, code}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        static string getPrimaryAccountCode();
        static void setPrimaryAccount(string & accountCode);
};

#endif
