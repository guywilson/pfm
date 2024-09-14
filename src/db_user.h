#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"
#include "base64.h"

using namespace std;

#ifndef __INCL_USER
#define __INCL_USER

class DBUser : public DBEntity {
    private:
        const char * sqlSelectByUser = 
                        "SELECT id, user_name, password, created, updated FROM user where user_name = '%s';";

        const char * sqlInsert = 
                        "INSERT INTO user (user_name, password, created, updated) VALUES ('%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE user SET user_name = '%s', password = '%s', updated = '%s' WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM user WHERE id = %lld;";

    public:
        string userName;
        string password;

        DBUser() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->userName = "";
            this->password = "";
        }

        void set(const DBUser & src) {
            DBEntity::set(src);

            this->userName = src.userName;
            this->password = src.password;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "user_name") {
                userName = column.getValue();
            }
            else if (column.getName() == "password") {
                password = base64::from_base64(column.getValue());
            }
        }

        const char * getTableName() override {
            return "user";
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                userName.c_str(),
                base64::to_base64(password).c_str(),
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
                userName.c_str(),
                base64::to_base64(password).c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        const char * getDeleteStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlDelete,
                id);

            return szStatement;
        }

        void retrieveByUser(string & user);
};

#endif
