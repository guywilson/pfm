#include <iostream>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "jfile.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_PAYEE
#define __INCL_PAYEE

class DBPayee : public DBEntity {
    private:
        const char * sqlSelectByCode = 
                        "SELECT id, code, name, created, updated FROM payee where code = '%s';";

        const char * sqlInsert = 
                        "INSERT INTO payee (code, name, created, updated) VALUES ('%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE payee SET code = '%s', name = '%s', updated = '%s' WHERE id = %lld;";

    public:
        string                  name;
        string                  code;

        DBPayee() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->name = "";
            this->code = "";
        }

        void set(const DBPayee & src) {
            DBEntity::set(src);

            this->name = src.name;
            this->code = src.code;
        }

        void set(JRecord & record) {
            this->code = record.get("code");
            this->name = record.get("name");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("code", this->code);
            r.add("name", this->name);

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "Description: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "code") {
                code = column.getValue();
            }
            else if (column.getName() == "name") {
                name = column.getValue();
            }
        }

        const char * getTableName() override {
            return "payee";
        }

        const char * getClassName() override {
            return "DBPayee";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dName = delimitSingleQuotes(name);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                code.c_str(),
                dName.c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dName = delimitSingleQuotes(name);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                code.c_str(),
                dName.c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        void retrieveByCode(string & code);
};

#endif
