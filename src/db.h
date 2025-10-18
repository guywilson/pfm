#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <pthread.h>
#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                   2048
#define SQL_ORDER_BY_BUFFER_LEN                      48

#define SQL_ROW_LIMIT                                80

typedef enum {
    sort_descending,
    sort_ascending
}
db_sort_t;

class pfm_id_t {
    private:
        string _value;
        bool _isNull;

        void inline checkForNumericChar(char c) {
            if (isprint(c)) {
                if (!isdigit(c)) {
                    throw pfm_validation_error("The ID value must be an integer");
                }
            }
        }

        void checkForNumericString(const string & s) {
            for (char c : s) {
                checkForNumericChar(c);
            }
        }

        void checkForNumericString(const char * s) {
            for (int i = 0;i < strlen(s);i++) {
                char c = s[i];

                checkForNumericChar(c);
            }
        }

    public:
        void set(sqlite3_int64 id) {
            char buffer[40];

            snprintf(buffer, sizeof(buffer), "%" PRId64, id);

            _value.assign(buffer);
            _isNull = false;
        }

        void set(const string & s) {
            if (s.compare("NULL") == 0) {
                clear();
                return;
            }

            checkForNumericString(s);

            _value = s;
            _isNull = false;
        }

        void set(const char * s) {
            if (strncmp(s, "NULL", 4) == 0) {
                clear();
                return;
            }
            
            checkForNumericString(s);

            _value = s;
            _isNull = false;
        }

        void clear() {
            _value = "NULL";
            _isNull = true;
        }

        string getValue() const {
            if (isNull()) {
                return "NULL";
            }

            return _value;
        }
        
        const char * c_str() const {
            return _value.c_str();
        }

        sqlite3_int64 intValue() {
            sqlite3_int64 id = (sqlite3_int64)strtoll(_value.c_str(), NULL, 10);
            return id;
        }

        pfm_id_t() {
            clear();
        }

        pfm_id_t(sqlite3_int64 id) {
            set(id);
        }

        pfm_id_t(const string & s) {
            set(s);
        }

        bool isNull() const {
            return _isNull;
        }

        pfm_id_t & operator=(const pfm_id_t & rhs) {
            if (this == &rhs) {
                return *this;
            }

            if (rhs.isNull()) {
                this->clear();
                return *this;
            }

            this->set(rhs.getValue());

            return *this;
        }

        pfm_id_t & operator=(const string & rhs) {
            this->set(rhs);
            return *this;
        }

        pfm_id_t & operator=(const char * rhs) {
            this->set(rhs);
            return *this;
        }

        pfm_id_t & operator=(const sqlite3_int64 rhs) {
            this->set(rhs);
            return *this;
        }

        bool operator==(const pfm_id_t & rhs) {
            if (rhs.isNull()) {
                return true;
            }

            return (this->_value.compare(rhs.getValue()) == 0);
        }

        bool operator==(const string & rhs) {
            return (this->_value.compare(rhs) == 0);
        }

        bool operator==(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) == 0);
        }

        bool operator!=(const pfm_id_t & rhs) {
            return (this->_value.compare(rhs.getValue()) != 0);
        }

        bool operator!=(const string & rhs) {
            return (this->_value.compare(rhs) != 0);
        }

        bool operator!=(const sqlite3_int64 rhs) {
            pfm_id_t id = rhs;
            return (this->_value.compare(id.getValue()) != 0);
        }
};

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
        void _executeSQLNoCallback(const char * sql);
        void _executeSQLCallback(const char * sql, vector<DBRow> * rows);

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

        int executeSelect(const char * statement, vector<DBRow> * rows);
        
        pfm_id_t executeInsert(const char * statement);

        void executeUpdate(const char * statement);
        void executeDelete(const char * statement);

        void begin();
        void commit();
        void rollback();
};

#endif
