#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __INCL_TRANSACTION_REPORT
#define __INCL_TRANSACTION_REPORT

class DBTransactionReport : public DBEntity {
    private:
        const char * sqlInsert = 
                        "INSERT INTO transaction_report (" \
                        "description," \
                        "sql_where_clause," \
                        "created," \
                        "updated) " \
                        "VALUES ('%s', '%s', '%s', '%s');";

        const char * sqlUpdate = 
                        "UPDATE transaction_report SET " \
                        "description = '%s'," \
                        "sql_where_clause = '%s'," \
                        "updated = '%s' " \
                        "WHERE id = %lld;";

    public:
        string                  description;
        string                  sqlWhereClause;

        DBTransactionReport() : DBEntity() {
            clear();
        }

        DBTransactionReport(const DBTransactionReport & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->description = "";
            this->sqlWhereClause = "";
        }

        void set(const DBTransactionReport & src) {
            DBEntity::set(src);

            this->description = src.description;
            this->sqlWhereClause =  src.sqlWhereClause;
        }

        void print() {
            DBEntity::print();

            cout << "Description: '" << description << "'" << endl;
            cout << "SQL: '" << sqlWhereClause << "'" << endl;
        }

        const char * getTableName() override {
            return "transaction_report";
        }

        const char * getClassName() override {
            return "DBTransactionReport";
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const char * getInsertStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dSQL = delimitSingleQuotes(sqlWhereClause);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlInsert,
                dDescription.c_str(),
                dSQL.c_str(),
                now.c_str(),
                now.c_str());

            return szStatement;
        }

        const char * getUpdateStatement() override {
            static char szStatement[SQL_STATEMENT_BUFFER_LEN];

            string now = StrDate::getTimestamp();

            string dDescription = delimitSingleQuotes(description);
            string dSQL = delimitSingleQuotes(sqlWhereClause);

            snprintf(
                szStatement, 
                SQL_STATEMENT_BUFFER_LEN,
                sqlUpdate,
                dDescription.c_str(),
                dSQL.c_str(),
                now.c_str(),
                id);

            return szStatement;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == "description") {
                description = column.getValue();
            }
            else if (column.getName() == "sql_where_clause") {
                sqlWhereClause = column.getValue();
            }
        }
};

#endif
