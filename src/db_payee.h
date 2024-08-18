#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "db.h"
#include "db_base.h"
#include "strdate.h"

using namespace std;

#ifndef __INCL_PAYEE
#define __INCL_PAYEE

class DBPayeeResult;

class DBPayee : public DBEntity {
    private:
        const char * sqlSelectByID = 
                        "SELECT id, code, name, created, updated FROM payee where id= %lld;";

        const char * sqlSelectByCode = 
                        "SELECT id, code, name, created, updated FROM payee where code = '%s';";

        const char * sqlSelectAll = 
                        "SELECT id, code, name, created, updated FROM payee;";

        const char * sqlInsert = 
                        "INSERT INTO payee (code, name, created, updated) VALUES ('%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE payee SET code = '%s', name = '%s', updated = '%s' WHERE id = %lld;";

        const char * sqlDelete = 
                        "DELETE FROM payee WHERE id = %lld;";

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

        void print() {
            DBEntity::print();

            cout << "Description: '" << name << "'" << endl;
            cout << "Code: '" << code << "'" << endl;
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                code.c_str(),
                name.c_str(),
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
                name.c_str(),
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

        void                retrieveByID(pfm_id_t id);
        void                retrieveByCode(string & code);
        DBPayeeResult       retrieveAll();
};

class DBPayeeResult : public DBResult {
    private:
        vector<DBPayee>           results;

    public:
        DBPayeeResult() : DBResult() {}

        void clear() {
            DBResult::clear();
            results.clear();
        }

        DBPayee getResultAt(int i) {
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
            DBPayee payee;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                if (column.getName() == "id") {
                    payee.id = column.getIDValue();
                }
                else if (column.getName() == "code") {
                    payee.code = column.getValue();
                }
                else if (column.getName() == "name") {
                    payee.name = column.getValue();
                }
                else if (column.getName() == "created") {
                    payee.createdDate = column.getValue();
                }
                else if (column.getName() == "updated") {
                    payee.updatedDate = column.getValue();
                }
            }

            results.push_back(payee);
            incrementNumRows();
        }
};

#endif
