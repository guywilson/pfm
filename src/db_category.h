#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CATEGORY
#define __INCL_CATEGORY

class DBCategory : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * description = "description";
            static constexpr const char * code = "code";
        };

    public:
        string description;
        string code;

        DBCategory() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->description = "";
            this->code = "";
        }

        void set(const DBCategory & src) {
            DBEntity::set(src);

            this->description = src.description;
            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->description = record.get("description");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", this->code);
            r.add("description", this->description);

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "Description: '" << description << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::code) {
                code = column.getValue();
            }
            else if (column.getName() == Columns::description) {
                description = column.getValue();
            }
        }

        const string getTableName() const override {
            return "category";
        }

        const string getClassName() const override {
            return "DBCategory";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::code, code},
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::code, code},
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByCode(string & code);
};

#endif
