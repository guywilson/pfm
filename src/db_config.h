#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db_base.h"

using namespace std;

#ifndef __INCL_CONFIG
#define __INCL_CONFIG

class DBConfig : public DBBase {
    public:
        DBConfig() : DBBase() {
            clear();
        }

        void clear(void) {
            DBBase::clear();

            this->key = "";
            this->value = "";
            this->description = "";
        }

        void set(const DBConfig & src) {
            DBBase::set(src);

            this->key.assign(src.key);
            this->value.assign(src.value);
            this->description.assign(src.description);
        }

        void print(void) {
            DBBase::print();

            cout << "Key: '" << key << "'" << endl;
            cout << "Value: '" << value << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        string                  key;
        string                  value;
        string                  description;
};

class DBConfigResult {
    public:
        DBConfigResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBConfig>        results;
};

#endif
