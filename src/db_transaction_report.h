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
    protected:
        struct Columns {
            static constexpr const char * description = "description";
            static constexpr const char * sqlWhereClause = "sql_where_clause";
        };

    public:
        string description;
        string sqlWhereClause;

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

        const string getInsertStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::sqlWhereClause, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<string, string>> columnValuePairs = {
                {Columns::description, delimitSingleQuotes(description)},
                {Columns::sqlWhereClause, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::description) {
                description = column.getValue();
            }
            else if (column.getName() == Columns::sqlWhereClause) {
                sqlWhereClause = column.getValue();
            }
        }
};

#endif
