#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_CATEGORY
#define __INCL_CATEGORY

class Category {
    public:
        Category() {
            clear();
        }

        void clear(void) {
            this->id = 0;

            this->description = "";
            this->code = "";
        }

        void setCategory(const Category & src) {
            this->id = src.id;

            this->description =     src.description;
            this->code =            src.code;
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

class CategoryResult {
    public:
        CategoryResult() {
            numRows = 0;
        }

        int                     numRows;

        vector<Category>        results;
};

#endif
