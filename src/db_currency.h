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
    private:
        const char * sqlInsert = 
                        "INSERT INTO currency (code, name, symbol, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE currency SET code = '%s', name = '%s', symbol = '%s', updated = '%s' WHERE id = %s;";

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
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dName = delimitSingleQuotes(name);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                code.c_str(),
                dName.c_str(),
                symbol.c_str(),
                now.c_str(),
                now.c_str());

            return string(szStatement);
        }

        const string getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dName = delimitSingleQuotes(name);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                dName.c_str(),
                symbol.c_str(),
                now.c_str(),
                id.c_str());

            return string(szStatement);
        }

        void retrieveByCode(string & code);
};

#endif
