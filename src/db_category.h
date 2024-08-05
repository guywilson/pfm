#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db_base.h"

using namespace std;

#ifndef __INCL_CATEGORY
#define __INCL_CATEGORY

class DBCategory : public DBEntity {
    public:
        DBCategory() : DBEntity() {
            clear();
        }

        void clear(void) {
            DBEntity::clear();

            this->description = "";
            this->code = "";
        }

        void set(const DBCategory & src) {
            DBEntity::set(src);

            this->description.assign(src.description);
            this->code.assign(src.code);
        }

        void print(void) {
            DBEntity::print();

            cout << "Description: '" << description << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

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
