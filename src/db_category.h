#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_CATEGORY
#define __INCL_CATEGORY

class DBCategory : public DBEntity {
    private:
        const char * sqlSelectByCode = 
                        "SELECT id, code, description, created, updated FROM category where code = '%s';";

        const char * sqlInsert = 
                        "INSERT INTO category (description, code, created, updated) VALUES ('%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE category SET code = '%s', description = '%s', updated = '%s' WHERE id = %lld;";

    public:
        string                  description;
        string                  code;

        DBCategory() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->description = "";
            this->code = "";
        }

        void set(const DBCategory & src) {
            DBEntity::set(src);

            this->description = src.description;
            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->description = record.get("description");
        }

        void print() {
            DBEntity::print();

            cout << "Description: '" << description << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "code") {
                code = column.getValue();
            }
            else if (column.getName() == "description") {
                description = column.getValue();
            }
        }

        const char * getTableName() override {
            return "category";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                description.c_str(),
                code.c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                description.c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        void retrieveByCode(string & code);
};

#endif
