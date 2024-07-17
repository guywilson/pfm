#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

class DBBase {
    public:
        DBBase() {
            clear();
        }

        void clear(void) {
            this->id = 0;
            this->sequence = 0;
            this->createdDate = "";
            this->updatedDate = "";
        }

        void set(const DBBase & src) {
            this->id = src.id;
            this->sequence = src.sequence;
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Sequence: " << sequence << endl;

            cout << "Created: " << createdDate << endl;
            cout << "Updated: " << updatedDate << endl;
        }

        sqlite3_int64           id;

        string                  createdDate;
        string                  updatedDate;

        uint32_t                sequence;           // Not persistent
};

#endif
