#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_ACCOUNT
#define __INCL_ACCOUNT

class DBAccount {
    public:
        DBAccount() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->name = "";
            this->code = "";
            this->openingBalance = 0.0;
            this->currentBalance = 0.0;
        }

        void setAccount(const DBAccount & src) {
            this->id = src.id;

            this->name =            src.name;
            this->code =            src.code;
            this->openingBalance =  src.openingBalance;
            this->currentBalance =  src.currentBalance;
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Name: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;

            cout << fixed << setprecision(2);
            cout << "Opening balance: " << openingBalance << endl;
            cout << "Current balance: " << currentBalance << endl;
        }

        sqlite3_int64           id;

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
