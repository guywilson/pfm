#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <sqlite3.h>

#include "pfm_error.h"

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

class DBResult;

typedef sqlite3_int64       pfm_id_t;

class DBEntity {
    private:
        unordered_map<string, string> fieldHashMap;

        pfm_id_t insert();
        void update();

    protected:
        void addColumnForField(string key, string columnName) {
            fieldHashMap.insert({key, columnName});
        }

        string getColumnForField(string key) {
            unordered_map<string, string>::const_iterator item = fieldHashMap.find(key);

            if (item != fieldHashMap.end()) {
                return item->second;
            }

            throw pfm_error(pfm_error::buildMsg("Field '%s' not found", key.c_str()));
        }

        virtual void beforeInsert() {
            return;
        }

        virtual void afterInsert() {
            return;
        }

        virtual void beforeUpdate() {
            return;
        }

        virtual void afterUpdate() {
            return;
        }

        virtual void beforeRemove() {
            return;
        }

        virtual void afterRemove() {
            return;
        }

    public:
        pfm_id_t id;

        string createdDate;
        string updatedDate;
        uint32_t sequence;           // Not persistent

        DBEntity() {
            clear();

            addColumnForField("id", "id");
            addColumnForField("createdDate", "created");
            addColumnForField("updatedDate", "updated");
        }

        DBEntity(const DBEntity & src) {
            set(src);
        }
        
        virtual ~DBEntity() {}

        virtual const char * getTableName() {
            return "";
        }

        virtual const char * getSelectByIDStatement() {
            static char statement[64];

            snprintf(statement, 64, "SELECT * FROM %s WHERE id = %lld;", getTableName(), id);
 
            return statement;
        }

        virtual const char * getInsertStatement() {
            return "";
        }

        virtual const char * getUpdateStatement() {
            return "";
        }

        virtual const char * getDeleteStatement() {
            return "";
        }

        void retrieveByID(DBResult * result);
        void remove();
        void save();

        void clear() {
            this->id = 0;
            this->sequence = 0;
            this->createdDate = "";
            this->updatedDate = "";
        }

        void set(const DBEntity & src) {
            this->id = src.id;
            this->createdDate = src.createdDate;
            this->updatedDate = src.updatedDate;
            this->sequence = src.sequence;
        }

        void print() {
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
            
            /*
            ** value will be NULL for blank NULLable columns...
            */
            if (value != NULL) {
                this->value = value;
            }
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

        pfm_id_t getIDValue() {
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
