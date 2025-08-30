#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "jfile.h"
#include "logger.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

#define LIMIT_CLAUSE_BUFFER_LEN                    32

template <class T> class DBResult;

class DBEntity {
    private:
        pfm_id_t insert();
        void update();

    protected:
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
        }

        DBEntity(const DBEntity & src) {
            set(src);
        }
        
        virtual ~DBEntity() {}

        virtual JRecord getRecord() {
            JRecord r;
            return r;
        }
        
        virtual const char * getTableName() {
            return "";
        }

        virtual const char * getClassName() {
            return "DBEntity";
        }

        virtual const char * getSelectByIDStatement(pfm_id_t key) {
            static char statement[64];

            snprintf(statement, 64, "SELECT * FROM %s WHERE id = %lld;", getTableName(), key); 
            return statement;
        }

        virtual const char * getSelectAllStatement() {
            static char statement[64];

            snprintf(statement, 64, "SELECT * FROM %s;", getTableName()); 
            return statement;
        }

        virtual const char * getDeleteByIDStatement(pfm_id_t key) {
            static char statement[64];

            snprintf(statement, 64, "DELETE FROM %s WHERE id = %lld;", getTableName(), key); 
            return statement;
        }

        virtual const char * getDeleteAllStatement() {
            static char statement[64];

            snprintf(statement, 64, "DELETE FROM %s;", getTableName()); 
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
        void remove(const char * statement);
        void removeAll();

        void save();
        void retrieve();
        void retrieve(pfm_id_t id);

        void clear() {
            Logger & log = Logger::getInstance();
            log.entry("%s:clear()", getClassName());

            this->id = 0;
            this->sequence = 0;
            this->createdDate = "";
            this->updatedDate = "";

            log.exit("%s:clear()", getClassName());
        }

        void set(const DBEntity & src) {
            Logger & log = Logger::getInstance();
            log.entry("%s:set()", getClassName());

            this->id = src.id;
            this->createdDate = src.createdDate;
            this->updatedDate = src.updatedDate;
            this->sequence = src.sequence;

            log.exit("%s:set()", getClassName());
        }

        virtual void assignColumn(DBColumn & column) {
            Logger & log = Logger::getInstance();
            log.entry("%s:assignColumn()", getClassName());

            if (column.getName() == "id") {
                id = column.getIDValue();
            }
            else if (column.getName() == "created") {
                createdDate = column.getValue();
            }
            else if (column.getName() == "updated") {
                updatedDate = column.getValue();
            }

            log.exit("%s:assignColumn()", getClassName());
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
            Logger & log = Logger::getInstance();
            log.entry("DBEntity::isYesNoBooleanValid()");

            if (ynValue == "Y" || ynValue == "N") {
                return true;
            }
            
            log.exit("DBEntity::isYesNoBooleanValid()");

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

        int size() {
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
            Logger & log = Logger::getInstance();
            log.entry("DBResult::clear()");

            Result::clear();
            results.clear();

            log.exit("DBResult::clear()");
        }

        void reverse() {
            Logger & log = Logger::getInstance();
            log.entry("DBResult::reverse()");

            list<T> l;

            for (int i = results.size() - 1;i >= 0;i--) {
                l.push_back(results[i]);
            }
            for (int i = 0;i < results.size();i++) {
                results[i] = l.front();
                l.pop_front();
            }

            log.exit("DBResult::reverse()");
        }
        
        int retrieve(const char * sqlStatement);
        int retrieveAll();

        T & at(int i) {
            if (size() > i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "at(): Index out of range: numRows: %d, requested row: %d", size(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        T & operator[](int i) {
            return at(i);
        }

        const T & operator[](int i) const {
            return at(i);
        }

        void addRow(T & entity) {
            results.push_back(entity);
        }

        void processRow(DBRow & row) {
            Logger & log = Logger::getInstance();
            log.entry("DBResult::processRow()");

            T entity;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                entity.assignColumn(column);
            }
            
            entity.onRowComplete(getSequence());
            incrementSequence();

            addRow(entity);
            incrementNumRows();

            log.exit("DBResult::processRow()");
        }
};

template <class T>
int DBResult<T>::retrieveAll() {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieveAll()");

    T entity;
    vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(entity.getSelectAllStatement(), &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieveAll()");

    return rowsRetrievedCount;
}

template <class T>
int DBResult<T>::retrieve(const char * sqlStatement) {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieve()");

    vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(sqlStatement, &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieve()");

    return rowsRetrievedCount;
}

#endif
