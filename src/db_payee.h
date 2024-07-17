#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db_base.h"

using namespace std;

#ifndef __INCL_PAYEE
#define __INCL_PAYEE

class DBPayee : public DBBase {
    public:
        DBPayee() : DBBase() {
            clear();
        }

        void clear(void) {
            DBBase::clear();

            this->name = "";
            this->code = "";
        }

        void set(const DBPayee & src) {
            DBBase::set(src);

            this->name =            src.name;
            this->code =            src.code;
        }

        void print(void) {
            DBBase::print();

            cout << "Description: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        string                  name;
        string                  code;
};

class DBPayeeResult {
    public:
        DBPayeeResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBPayee>           results;
};

#endif
