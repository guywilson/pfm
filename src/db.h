#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <pthread.h>
#include <sqlcipher/sqlite3.h>

#include "pfm_id.h"
#include "pfm_error.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                   2048
#define SQL_ORDER_BY_BUFFER_LEN                      48

#define SQL_ROW_LIMIT                                80

class DBColumn {
    private:
        string name;
        string value;
        bool isNull;

    public:
        DBColumn(const char * name, const char * value) {
            this->name = name;
            
            /*
            ** value will be NULL for blank NULLable columns...
            */
            if (value != NULL) {
                this->value = value;
                isNull = false;
            }
            else {
                isNull = true;
            }
        }

        string getName() {
            return name;
        }

        string getValue() {
            return value;
        }

        double doubleValue() {
            return strtod(getValue().c_str(), NULL);
        }

        long getIntValue() {
            return strtol(getValue().c_str(), NULL, 10);
        }

        unsigned long getUnsignedIntValue() {
            return strtoul(getValue().c_str(), NULL, 10);
        }

        bool getBoolValue() {
            return ((value[0] == 'Y' || value[0] == 'y') ? true : false);
        }

        pfm_id_t getIDValue() {
            pfm_id_t id;

            if (!isNull) {
                id.set(getValue());
            }

            return id;
        }
};

class DBRow {
    private:
        vector<DBColumn>  columns;

    public:
        DBRow(int numColumns, vector<DBColumn> & columnVector) {
            for (size_t i = 0;i < columnVector.size();i++) {
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

class PFM_DB {
    public:
        static PFM_DB & getInstance() {
            static PFM_DB instance;
            return instance;
        }

    private:
        sqlite3 * dbHandle;
        bool isTransactionActive;
        string databaseName;
        pthread_mutex_t mutex;
        Logger & log = Logger::getInstance();

        PFM_DB() {
            isTransactionActive = false;
        }

        int openReadWrite(const string & dbName);
        void createDB(const string & dbName);
        bool isNewFileRequired(int errorCode);

        void encryptKey(const string & key, uint8_t * buffer, int bufferLength);
        string decryptKey(uint8_t * buffer, int bufferLength);
        void applyDatabaseKey(const string & dbName, const string & key);
        string readKeyFile(const string & keyFileName);

        void createAccessKeyRecord(const string & key);
        void _executeSQLNoCallback(const string & sql);
        void _executeSQLCallback(const string & sql, vector<DBRow> * rows);

        bool getIsTransactionActive();
        void setIsTransactionActive();
        void clearIsTransactionActive();

        void createSchema();

        void createDefaultCategories();
        void createDefaultConfig();
        void createCurrencies();

    public:
        ~PFM_DB() {}

        void open(const string & dbName);
        void open();
        void close();

        string getKey(const string & prompt);

        void saveKeyFile(const string & key);

        void changePassword();

        void createTable(const char * sql);
        void createView(const char * sql);

        int executeSelect(const string & statement, vector<DBRow> * rows);
        
        pfm_id_t executeInsert(const string & statement);

        void executeUpdate(const string & statement);
        void executeDelete(const string & statement);

        void begin();
        void commit();
        void rollback();
};

#endif
