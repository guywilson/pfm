#include <string>
#include <stdint.h>

#include <sqlite3.h>

#include "db_criteria.h"
#include "db_base.h"

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
        PFM_DB() {}

        sqlite3 * dbHandle;

        void createSchema();

    public:
        ~PFM_DB();

        bool open(string dbName);

        void begin();
        void commit();
        void rollback();

        template <class T>
        int executeSelect(const char * sqlStatement, T * result);

        pfm_id_t executeInsert(const char * sqlStatement);
        void executeUpdate(const char * sqlStatement);
        void executeDelete(const char * sqlStatement);
};

#endif
