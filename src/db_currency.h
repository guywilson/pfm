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
        const char * sqlSelectByID = 
                        "SELECT id, code, name, symbol, created, updated FROM currency where id= %lld;";

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

        void                retrieveByID(pfm_id_t id);
        void                retrieveByCode(string & code);
        DBCurrencyResult    retrieveAll();
};

class DBCurrencyResult : public DBResult {
    private:
        vector<DBCurrency>      results;

    public:
        DBCurrencyResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBCurrency getResultAt(int i) {
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
            DBCurrency currency;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    currency.id = column.getIDValue();
                }
                else if (column.getName() == "code") {
                    currency.code = column.getValue();
                }
                else if (column.getName() == "name") {
                    currency.name = column.getValue();
                }
                else if (column.getName() == "symbol") {
                    currency.symbol = column.getValue();
                }
                else if (column.getName() == "created") {
                    currency.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    currency.updatedDate = column.getValue();
                }
            }
            
            results.push_back(currency);
            incrementNumRows();
        }
};

#endif
