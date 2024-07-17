#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db_base.h"

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccount : public DBBase {
    public:
        DBAccount() : DBBase() {
            clear();
        }

        void clear(void) {
            DBBase::clear();

            this->name = "";
            this->code = "";
            this->openingBalance = 0.0;
            this->currentBalance = 0.0;
        }

        void set(const DBAccount & src) {
            DBBase::set(src);

            this->name =            src.name;
            this->code =            src.code;
            this->openingBalance =  src.openingBalance;
            this->currentBalance =  src.currentBalance;
        }

        void print(void) {
            DBBase::print();

            cout << "Name: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Opening balance: " << openingBalance << endl;
            cout << "Current balance: " << currentBalance << endl;
        }

        string                  name;
        string                  code;
        double                  openingBalance;
        double                  currentBalance;
};

class DBAccountResult {
    public:
        DBAccountResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBAccount>         results;
};

#endif
