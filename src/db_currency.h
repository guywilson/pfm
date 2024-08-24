#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CURRENCY
#define __INCL_CURRENCY

class DBCurrencyResult;

class DBCurrency : public DBEntity {
    private:
        const char * sqlSelectByCode = 
                        "SELECT id, code, name, symbol, created, updated FROM currency where code = '%s';";

        const char * sqlSelectAll = 
                        "SELECT id, code, name, symbol, created, updated FROM currency;";

        const char * sqlInsert = 
                        "INSERT INTO currency (code, name, symbol, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE currency SET code = '%s', name = '%s', symbol = '%s', updated = '%s' WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM currency WHERE id = %lld;";

    public:
        string                  code;
        string                  name;
        string                  symbol;

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
            if (column.getName() == "code") {
                code = column.getValue();
            }
            else if (column.getName() == "name") {
                name = column.getValue();
            }
            else if (column.getName() == "symbol") {
                symbol = column.getValue();
            }
        }

        const char * getTableName() override {
            return "currency";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                code.c_str(),
                name.c_str(),
                symbol.c_str(),
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
                code.c_str(),
                name.c_str(),
                symbol.c_str(),
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

        void retrieveByCode(string & code);
        DBResult<DBCurrency> retrieveAll();
};

#endif
