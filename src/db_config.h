#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CONFIG
#define __INCL_CONFIG

class DBConfig : public DBEntity {
    private:
        const char * sqlInsert = 
                        "INSERT INTO config (key, value, description, is_read_only, is_visible, created, updated) VALUES ('%s', '%s', '%s', 'N', 'Y', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE config SET key = '%s', value = '%s', description = '%s', updated = '%s' WHERE id = %s;";

    public:
        string                  key;
        string                  value;
        string                  description;
        bool                    isReadOnly;
        bool                    isVisible;

        DBConfig() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->key = "";
            this->value = "";
            this->description = "";
            this->isReadOnly = false;
            this->isVisible = true;
        }

        void set(const DBConfig & src) {
            DBEntity::set(src);

            this->key.assign(src.key);
            this->value.assign(src.value);
            this->description.assign(src.description);
            this->isReadOnly = src.isReadOnly;
            this->isVisible = src.isVisible;
        }

        void print() {
            DBEntity::print();

            cout << "Key: '" << key << "'" << endl;
            cout << "Value: '" << value << "'" << endl;
            cout << "Description: '" << description << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "key") {
                key = column.getValue();
            }
            else if (column.getName() == "value") {
                value = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "is_read_only") {
                isReadOnly = column.getBoolValue();
            }
            else if (column.getName() == "is_visible") {
                isVisible = column.getBoolValue();
            }
        }

        const char * getTableName() override {
            return "config";
        }

        const char * getClassName() override {
            return "DBConfig";
        }

        const string getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dValue = delimitSingleQuotes(value);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                key.c_str(),
                dValue.c_str(),
                dDescription.c_str(),
                now.c_str(),
                now.c_str());

            return string(szStatement);
        }

        const string getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dValue = delimitSingleQuotes(value);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                key.c_str(),
                dValue.c_str(),
                dDescription.c_str(),
                now.c_str(),
                id.c_str());

            return string(szStatement);
        }

        void retrieveByKey(const string & key);
        DBResult<DBConfig> retrieveAllVisible();
};

#endif
