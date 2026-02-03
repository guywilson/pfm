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
            static constexpr ColumnType description_type = ColumnType::TEXT;

            static constexpr const char * sqlWhereClause = "sql_where_clause";
            static constexpr ColumnType sqlWhereClause_type = ColumnType::TEXT;
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

        void set(JRecord & record) {
            this->description = record.get("description");
            this->sqlWhereClause = record.get("sql");
        }

        JRecord getRecord() override  {
            JRecord r;

            r.add("description", description);
            r.add("sql", sqlWhereClause);

            return r;
        }

        void print() {
            DBEntity::print();

            cout << "Description: '" << description << "'" << endl;
            cout << "SQL: '" << sqlWhereClause << "'" << endl;
        }

        const string getTableName() const override {
            return "transaction_report";
        }

        const string getClassName() const override {
            return "DBTransactionReport";
        }

        const string getJSONArrayName() const override {
            return "reports";
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        const string getInsertStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::sqlWhereClause, Columns::sqlWhereClause_type}, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const string getUpdateStatement() override {
            vector<pair<ColumnDef, string>> columnValuePairs = {
                {{Columns::description, Columns::description_type}, delimitSingleQuotes(description)},
                {{Columns::sqlWhereClause, Columns::sqlWhereClause_type}, delimitSingleQuotes(sqlWhereClause)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(ofstream & os) override {
            DBResult<DBTransactionReport> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << endl;
            }

            os.flush();
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
