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
        sqlite3_int64           id;

        string                  createdDate;
        string                  updatedDate;

        uint32_t                sequence;           // Not persistent

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
};

class DBColumn {
    private:
        string name;
        string value;

    public:
        DBColumn(const char * name, const char * value) {
            this->name = name;
            this->value = value;
        }

        string getName() {
            return name;
        }

        string getValue() {
            return value;
        }

        double getDoubleValue() {
            return strtod(value.c_str(), NULL);
        }

        long getIntValue() {
            return strtol(value.c_str(), NULL, 10);
        }

        unsigned long getUnsignedIntValue() {
            return strtoul(value.c_str(), NULL, 10);
        }

        sqlite3_int64 getIDValue() {
            return strtoll(value.c_str(), NULL, 10);
        }
};

class DBRow {
    private:
        vector<DBColumn>  columns;

    public:
        DBRow(int numColumns, vector<DBColumn> & columnVector) {
            for (int i = 0;i < columnVector.size();i++) {
                columns.push_back(columnVector[i]);
            }
        }

        size_t getNumColumns() {
            return columns.size();
        }

        DBColumn getColumnAt(int i) {
            return columns[i];
        }
};

class DBResult {
    protected:
        int numRows;

    public:
        DBResult() {
            numRows = 0;
        }

        void clear() {
            numRows = 0;
        }

        int getNumRows() {
            return numRows;
        }

        void incrementNumRows() {
            numRows++;
        }

        void addRow(DBBase & entity) {
            return;
        }

        virtual void processRow(DBRow & row) {
            cout << "Called base class processRow()!!" << endl;
            return;
        }
};

#endif
