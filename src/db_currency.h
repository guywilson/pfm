#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db_base.h"

using namespace std;

#ifndef __INCL_CURRENCY
#define __INCL_CURRENCY

class DBCurrency : public DBEntity {
    public:
        DBCurrency() : DBEntity() {
            clear();
        }

        void clear(void) {
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

        void print(void) {
            DBEntity::print();

            cout << "Code: '" << code << "'" << endl;
            cout << "Name: '" << name << "'" << endl;
            cout << "Symbol: '" << symbol << "'" << endl;
        }

        string                  code;
        string                  name;
        string                  symbol;
};

class DBCurrencyResult {
    public:
        DBCurrencyResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBCurrency>      results;
};

#endif
