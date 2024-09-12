#include <string>
#include <vector>
#include <stdint.h>

#include <pthread.h>
#include <sqlite3.h>

#include "pfm_error.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_DB
#define __INCL_DB

#define SQLITE_ERROR_BUFFER_LEN                     512
#define SQL_STATEMENT_BUFFER_LEN                    512
#define SQL_ROW_LIMIT                                50

typedef enum {
    sort_descending,
    sort_ascending
}
db_sort_t;

typedef sqlite3_int64 pfm_id_t;

class DBColumn {
    private:
        string name;
        string value;

        static bool isCharPermitted(char ch) {
            const char * permittedCharset = " -0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:;.,";

            for (int i = 0;i < strlen(permittedCharset);i++) {
                if (ch == permittedCharset[i]) {
                    return true;
                }
            }

            return false;
        }

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

        static void validateStringValue(const string & value) {
            for (int i = 0;i < value.length();i++) {
                if (!isCharPermitted(value[i])) {
                    throw pfm_error(pfm_error::buildMsg("Invalid char '%c' in value '%s'", value[i], value.c_str()));
                }
            }
        }

        static string encrypt(string & value, string & key) {
            string out = value;

            int j = 0;

            for (int i = 0;i < value.length();i++) {
                if (j == key.length()) {
                    j = 0;
                }
                
                out[i] = value[i] ^ key[j++];
            }

            return out;
        }

        static string decrypt(string & value, string & key) {
            return encrypt(value, key);
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

class PFM_DB {
    public:
        static PFM_DB & getInstance() {
            static PFM_DB instance;
            return instance;
        }

    private:
        sqlite3 * dbHandle;
        bool isTransactionActive;
        pthread_mutex_t mutex;
        Logger & log = Logger::getInstance();

        PFM_DB() {
            isTransactionActive = false;
        }

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
        ~PFM_DB();

        bool open(string dbName);

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
