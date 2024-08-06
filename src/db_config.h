#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CONFIG
#define __INCL_CONFIG

class DBConfigResult;

class DBConfig : public DBEntity {
    private:
        const char * sqlSelectByID = 
                        "SELECT id, key, value, description, created, updated FROM config where id= %lld;";

        const char * sqlSelectByKey = 
                        "SELECT id, key, value, description, created, updated FROM config where key = '%s';";

        const char * sqlSelectAll = 
                        "SELECT id, key, value, description, created, updated FROM config;";

        const char * sqlInsert = 
                        "INSERT INTO config (key, value, description, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE config SET key = '%s', value = '%s', description = '%s', updated = '%s' WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM config WHERE id = %lld;";

    public:
        string                  key;
        string                  value;
        string                  description;

        DBConfig() : DBEntity() {
            clear();
        }

        void clear(void) {
            DBEntity::clear();

            this->key = "";
            this->value = "";
            this->description = "";
        }

        void set(const DBConfig & src) {
            DBEntity::set(src);

            this->key.assign(src.key);
            this->value.assign(src.value);
            this->description.assign(src.description);
        }

        void print(void) {
            DBEntity::print();

            cout << "Key: '" << key << "'" << endl;
            cout << "Value: '" << value << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                key.c_str(),
                value.c_str(),
                description.c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                key.c_str(),
                value.c_str(),
                description.c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::now();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void                retrieveByID(sqlite3_int64 id);
        void                retrieveByKey(string & key);
        DBConfigResult      retrieveAll(void);
};

class DBConfigResult : public DBResult {
    private:
        vector<DBConfig>        results;

    public:
        DBConfigResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBConfig getResultAt(int i) {
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

        void processRow(DBRow & row) {
            DBConfig config;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    config.id = column.getIDValue();
                }
                else if (column.getName() == "key") {
                    config.key = column.getValue();
                }
                else if (column.getName() == "value") {
                    config.value = column.getValue();
                }
                else if (column.getName() == "description") {
                    config.description = column.getValue();
                }
                else if (column.getName() == "created") {
                    config.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    config.updatedDate = column.getValue();
                }
            }
            
            results.push_back(config);
            incrementNumRows();
        }
};

#endif
