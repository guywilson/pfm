#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_CATEGORY
#define __INCL_CATEGORY

class DBCategory {
    public:
        DBCategory() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->description = "";
            this->code = "";
        }

        void setCategory(const DBCategory & src) {
            this->id = src.id;

            this->description.assign(src.description);
            this->code.assign(src.code);
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Description: '" << description << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        sqlite3_int64           id;

        string                  description;
        string                  code;
};

class DBCategoryResult {
    public:
        DBCategoryResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
            results.clear();
        }

        int                     numRows;

        vector<DBCategory>        results;
};

#endif
