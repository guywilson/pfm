#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdint.h>

#include <sqlite3.h>

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

class DBEntity {
    private:
        sqlite3_int64   insert(void);
        void            update(void);

    public:
        sqlite3_int64           id;

        string                  createdDate;
        string                  updatedDate;
        uint32_t                sequence;           // Not persistent

        DBEntity() {
            clear();
        }

        virtual ~DBEntity() {}

        virtual const char * getInsertStatement(void) {
            return "";
        }

        virtual const char * getUpdateStatement(void) {
            return "";
        }

        virtual const char * getDeleteStatement(void) {
            return "";
        }

        void remove(void);
        void save(void);

        void clear(void) {
            this->id = 0;
            this->sequence = 0;
            this->createdDate = "";
            this->updatedDate = "";
        }

        void set(const DBEntity & src) {
            this->id = src.id;
            this->sequence = src.sequence;
        }

        void print(void) {
            cout << "ID: " << id << endl;
            cout << "Sequence: " << sequence << endl;

            cout << "Created: " << createdDate << endl;
            cout << "Updated: " << updatedDate << endl;
        }

        static bool isYesNoBooleanValid(string & ynValue) {
            if (ynValue == "Y" || ynValue == "N") {
                return true;
            }
            
            return false;
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

        bool getBoolValue() {
            return ((value[0] == 'Y' || value[0] == 'y') ? true : false);
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
        int sequenceCounter;

    public:
        DBResult() {
            clear();
        }

        void clear() {
            numRows = 0;
            sequenceCounter = 1;
        }

        int getNumRows() {
            return numRows;
        }

        void incrementNumRows() {
            numRows++;
        }

        void addRow(DBEntity & entity) {
            return;
        }

        virtual void processRow(DBRow & row) {
            return;
        }
};

#endif
