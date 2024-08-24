#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <sqlite3.h>

#include "db.h"
#include "logger.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

template <class T> class DBResult;

typedef sqlite3_int64       pfm_id_t;

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

        virtual const char * getSelectByIDStatement(pfm_id_t key) {
            static char statement[64];

            snprintf(statement, 64, "SELECT * FROM %s WHERE id = %lld;", getTableName(), key);
 
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

        virtual void assignColumn(DBColumn & column) {
            if (column.getName() == "id") {
                id = column.getIDValue();
            }
            else if (column.getName() == "created") {
                createdDate = column.getValue();
            }
            else if (column.getName() == "updated") {
                updatedDate = column.getValue();
            }
        }

        virtual void onRowComplete(int sequence) {
            return;
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

class Result {
    private:
        int numRows;
        int sequenceCounter;

    public:
        Result() {
            clear();
        }

        virtual void clear() {
            numRows = 0;
            sequenceCounter = 1;
        }

        int getNumRows() {
            return numRows;
        }

        void incrementNumRows() {
            numRows++;
        }
        
        void incrementSequence() {
            sequenceCounter++;
        }

        int getSequence() {
            return sequenceCounter;
        }

        virtual void processRow(DBRow & row) {}
};

template <class T>
class DBResult : public Result {
    private:
        vector<T> results;

    public:
        DBResult() : Result() {
            clear();
        }

        void clear() override {
            Result::clear();

            results.clear();
        }

        int executeSelect(const char * sqlStatement);

        T retrieveByID(pfm_id_t id);
        void retrieveAll();

        T getResultAt(int i) {
            if (getNumRows() > i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "getResultAt(): Index out of range: numRows: %d, requested row: %d", getNumRows(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        void addRow(T & entity) {
            results.push_back(entity);
        }

        void processRow(DBRow & row) override {
            T entity;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                entity.assignColumn(column);
            }
            
            incrementSequence();
            entity.onRowComplete(getSequence());

            results.push_back(entity);
            incrementNumRows();
        }
};

static inline int _retrieveCallback(void * p, int numColumns, char ** columns, char ** columnNames) {
    vector<DBRow> * rows = (vector<DBRow> *)p;
    vector<DBColumn> columnVector;

    for (int i = 0;i < numColumns;i++) {
        DBColumn column(columnNames[i], columns[i]);
        columnVector.push_back(column);
    }

    DBRow row(numColumns, columnVector);

    rows->push_back(row);

    return SQLITE_OK;
}

template <class T>
int DBResult<T>::executeSelect(const char * sqlStatement) {
    vector<DBRow> rows;

    Logger & log = Logger::getInstance();

    log.logDebug("Executing SELECT statement '%s'", sqlStatement);

    PFM_DB & db = PFM_DB::getInstance();

    char * pszErrorMsg;

    int error = sqlite3_exec(
                db.getHandle(), 
                sqlStatement, 
                _retrieveCallback, 
                &rows, 
                &pszErrorMsg);

    if (error) {
        log.logError("Failed to execute statement '%s' with error '%s'", sqlStatement, pszErrorMsg);

        throw pfm_error(
                pfm_error::buildMsg(
                    "Failed to execute statement '%s': %s",
                    sqlStatement, 
                    pszErrorMsg), 
                __FILE__, 
                __LINE__);
    }
    else {
        for (int i = 0;i < rows.size();i++) {
            processRow(rows[i]);
        }
    }

    return this->getNumRows();
}

template <class T>
T DBResult<T>::retrieveByID(pfm_id_t id) {
    T entity;

    int rowsRetrievedCount = executeSelect(entity.getSelectByIDStatement(id));

    if (rowsRetrievedCount != 1) {
        throw pfm_error(
                pfm_error::buildMsg("Expected exactly 1 row, got %d", rowsRetrievedCount), 
                __FILE__, 
                __LINE__);
    }

    return getResultAt(0);
}

template <class T>
void DBResult<T>::retrieveAll() {

}

#endif
