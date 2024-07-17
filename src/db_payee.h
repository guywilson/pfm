#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_PAYEE
#define __INCL_PAYEE

class DBPayee {
    public:
        DBPayee() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->name = "";
            this->code = "";
        }

        void setPayee(const DBPayee & src) {
            this->id = src.id;

            this->name =            src.name;
            this->code =            src.code;
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Description: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        sqlite3_int64           id;

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
