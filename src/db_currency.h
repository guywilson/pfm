#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CURRENCY
#define __INCL_CURRENCY

class DBCurrency : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * code = "code";
            static constexpr const char * name = "name";
            static constexpr const char * symbol = "symbol";
        };

    public:
        string code;
        string name;
        string symbol;

        DBCurrency() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->code = "";
            this->name = "";
            this->symbol = "";
        }

        void set(const DBCurrency & src) {
            DBEntity::set(src);

            this->code.assign(src.code);
            this->name.assign(src.name);
            this->symbol.assign(src.symbol);
        }

        void print() {
            DBEntity::print();

            cout << "Code: '" << code << "'" << endl;
            cout << "Name: '" << name << "'" << endl;
            cout << "Symbol: '" << symbol << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::code) {
                code = column.getValue();
            }
            else if (column.getName() == Columns::name) {
                name = column.getValue();
            }
            else if (column.getName() == Columns::symbol) {
                symbol = column.getValue();
            }
        }

        const char * getTableName() override {
            return "currency";
        }

        const char * getClassName() override {
            return "DBCurrency";
        }

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::code, code},
                {Columns::name, delimitSingleQuotes(name)},
                {Columns::symbol, symbol}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::code, code},
                {Columns::name, delimitSingleQuotes(name)},
                {Columns::symbol, symbol}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void retrieveByCode(string & code);
};

#endif
