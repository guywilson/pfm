#include <string>
#include <stdint.h>

#include <pthread.h>
#include <sqlite3.h>

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

        void executeSQL(const char * sql);

        bool getIsTransactionActive();
        void setIsTransactionActive();
        void clearIsTransactionActive();

        void createTable(const char * sql);
        void createView(const char * sql);
        void createSchema();

        void createDefaultCategories();
        void createDefaultConfig();
        void createCurrencies();

    public:
        ~PFM_DB();

        bool open(string dbName);

        sqlite3 * getHandle();

        void begin();
        void commit();
        void rollback();
};

#endif
