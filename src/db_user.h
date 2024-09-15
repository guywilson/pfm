#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_USER
#define __INCL_USER

class DBUser : public DBEntity {
    private:
        const char * sqlSelectByUser = 
                        "SELECT id, user_name, password, has_password, created, updated FROM user where user_name = '%s';";

        const char * sqlInsert = 
                        "INSERT INTO user (user_name, password, has_password, created, updated) VALUES ('%s', '%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE user SET user_name = '%s', password = '%s', has_password = '%s', updated = '%s' WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM user WHERE id = %lld;";

        const char * getHasPassword() {
            return (hasPassword ? "Y" : "N");
        }

        const char * getValidationErrorMsg() {
            return "Invalid user name or password";
        }

    public:
        string userName;
        string password;
        bool hasPassword;

        DBUser() : DBEntity() {
            clear();
        }

        void clear() {
            DBEntity::clear();

            this->userName = "";
            this->password = "";
            this->hasPassword = false;
        }

        void set(const DBUser & src) {
            DBEntity::set(src);

            this->userName = src.userName;
            this->password = src.password;
            this->hasPassword = src.hasPassword;
        }

        void print() {
            DBEntity::print();

            cout << "UserName: '" << userName << "'" << endl;
            cout << "Password: '" << password << "'" << endl;
            cout << "HasPassword: '" << (hasPassword ? "Y" : "N") << "'" << endl;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "user_name") {
                userName = column.getValue();
            }
            else if (column.getName() == "password") {
                password = column.getValue();
            }
            else if (column.getName() == "has_password") {
                hasPassword = column.getBoolValue();
            }
        }

        void checkPassword(string & passwordToCheck) {
            if (password.compare(passwordToCheck) != 0) {
                throw pfm_error(getValidationErrorMsg());
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
                password.c_str(),
                getHasPassword(),
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
                password.c_str(),
                getHasPassword(),
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
