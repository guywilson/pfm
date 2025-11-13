#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <deque>
#include <map>
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db.h"
#include "strdate.h"
#include "money.h"
#include "jfile.h"
#include "logger.h"
#include "pfm_error.h"

using namespace std;

#ifndef __INCL_DB_BASE
#define __INCL_DB_BASE

#define SQL_STATEMENT_LENGTH                   256
#define LIMIT_CLAUSE_BUFFER_LEN                 32
#define SINGLE_QUOTE_CHAR                       39
#define NULL_ROW_LIMIT                          -1

template <class T> class DBResult;

class DBCriteria {
    private:
        deque<string> whereClauses;
        deque<string> orderClauses;
        multimap<string, string> inClauses;
        int rowLimit;

        string appendString(string & clause, const string & value) {
            clause.append("'");
            clause.append(value);
            clause.append("'");

            return clause;
        }

        string appendBoolean(string & clause, const bool value) {
            clause.append("'");
            clause.append(value ? "Y" : "N");
            clause.append("'");

            return clause;
        }

    public:
        enum sql_operator {
            less_than,
            less_than_or_equal,
            greater_than,
            greater_than_or_equal,
            equal_to,
            not_equal_to,
            like,
            not_like,
            is_null,
            is_not_null
        };

        enum sql_order {
            descending,
            ascending
        };

        DBCriteria() {
            clear();
        }

        void clear() {
            whereClauses.clear();
            orderClauses.clear();
            rowLimit = NULL_ROW_LIMIT;
        }

        void add(const string & columnName, enum sql_operator op, const string & value) {
            string clause = columnName;

            switch (op) {
                case less_than:
                    clause.append(" < ");
                    clause = appendString(clause, value);
                    break;

                case less_than_or_equal:
                    clause.append(" <= ");
                    clause = appendString(clause, value);
                    break;

                case greater_than:
                    clause.append(" > ");
                    clause = appendString(clause, value);
                    break;

                case greater_than_or_equal:
                    clause.append(" >= ");
                    clause = appendString(clause, value);
                    break;

                case equal_to:
                    clause.append(" = ");
                    clause = appendString(clause, value);
                    break;

                case not_equal_to:
                    clause.append(" != ");
                    clause = appendString(clause, value);
                    break;

                case like:
                    clause.append(" LIKE ");
                    clause = appendString(clause, value);
                    break;

                case not_like:
                    clause.append(" NOT LIKE ");
                    clause = appendString(clause, value);
                    break;

                case is_null:
                    clause.append(" IS NULL");
                    break;

                case is_not_null:
                    clause.append(" IS NOT NULL");
                    break;
            }

            whereClauses.push_back(clause);
        }

        void add(const string & columnName, enum sql_operator op, const StrDate & value) {
            add(columnName, op, value.shortDate());
        }

        void add(const string & columnName, enum sql_operator op, const Money & value) {
            string clause = columnName;

            switch (op) {
                case less_than:
                    clause.append(" < ");
                    clause.append(value.rawStringValue());
                    break;

                case less_than_or_equal:
                    clause.append(" <= ");
                    clause.append(value.rawStringValue());
                    break;

                case greater_than:
                    clause.append(" > ");
                    clause.append(value.rawStringValue());
                    break;

                case greater_than_or_equal:
                    clause.append(" >= ");
                    clause.append(value.rawStringValue());
                    break;

                case equal_to:
                    clause.append(" = ");
                    clause.append(value.rawStringValue());
                    break;

                case not_equal_to:
                    clause.append(" != ");
                    clause.append(value.rawStringValue());
                    break;

                case is_null:
                    clause.append(" IS NULL");
                    break;

                case is_not_null:
                    clause.append(" IS NOT NULL");
                    break;

                default:
                    throw pfm_error("DBCriteria::add() - Illegal operation for a Money field");
            }

            whereClauses.push_back(clause);
        }

        void add(const string & columnName, enum sql_operator op, const pfm_id_t & id) {
            string clause = columnName;

            switch (op) {
                case equal_to:
                    clause.append(" = ");
                    clause.append(id.getValue());
                    break;

                case not_equal_to:
                    clause.append(" != ");
                    clause.append(id.getValue());
                    break;

                case is_null:
                    clause.append(" IS NULL");
                    break;

                case is_not_null:
                    clause.append(" IS NOT NULL");
                    break;

                default:
                    throw pfm_error("DBCriteria::add() - Illegal operation for an ID field");
            }

            whereClauses.push_back(clause);
        }

        void addFirst(const string & columnName, enum sql_operator op, const pfm_id_t & id) {
            string clause = columnName;

            switch (op) {
                case equal_to:
                    clause.append(" = ");
                    clause.append(id.getValue());
                    break;

                case not_equal_to:
                    clause.append(" != ");
                    clause.append(id.getValue());
                    break;

                case is_null:
                    clause.append(" IS NULL");
                    break;

                case is_not_null:
                    clause.append(" IS NOT NULL");
                    break;

                default:
                    throw pfm_error("DBCriteria::addFirst() - Illegal operation for an ID field");
            }

            whereClauses.push_front(clause);
        }

        void add(const string & columnName, const bool value) {
            string clause = columnName;

            clause.append(" = ");
            clause = appendBoolean(clause, value);

            whereClauses.push_back(clause);
        }

        void addToInClause(const string & columnName, const string & value) {
            inClauses.insert(pair{columnName, value});
        }

        void addToInClause(const string & columnName, const StrDate & value) {
            addToInClause(columnName, value.shortDate());
        }

        void addToInClause(const string & columnName, const pfm_id_t & value) {
            addToInClause(columnName, value.getValue());
        }

        void endInClause_string(const string & columnName) {
            string clause = columnName + " IN (";

            auto range = inClauses.equal_range(columnName);

            size_t j = 0;
            for (auto i = range.first; i != range.second; ++i, j++) {
                appendString(clause, i->second);

                if (j < (inClauses.count(columnName) - 1)) {
                    clause.append(", ");
                }
            }

            clause += ')';

            whereClauses.push_back(clause);
        }

        void endInClause_StrDate(const string & columnName) {
            endInClause_string(columnName);
        }

        void endInClause_id(const string & columnName) {
            string clause = columnName + " IN (";

            auto range = inClauses.equal_range(columnName);

            size_t j = 0;
            for (auto i = range.first; i != range.second; ++i, j++) {
                clause.append(i->second);

                if (j < (inClauses.count(columnName) - 1)) {
                    clause.append(", ");
                }
            }

            clause += ')';

            whereClauses.push_back(clause);
        }

        void addOrderBy(const string & columnName, enum sql_order order) {
            string clause = columnName + ' ';

            switch (order) {
                case descending:
                    clause.append("DESC");
                    break;

                case ascending:
                    clause.append("ASC");
                    break;
            }

            orderClauses.push_back(clause);
        }

        void setRowLimit(int numRows) {
            rowLimit = numRows;
        }

        const string getWhereClause() {
            if (whereClauses.empty()) {
                return "";
            }

            string where = " WHERE ";

            while (!whereClauses.empty()) {
                where += whereClauses.front();
                whereClauses.pop_front();

                if (!whereClauses.empty()) {
                    where += " AND ";
                }
            }

            return where;
        }

        const string getOrderBy() {
            if (orderClauses.empty()) {
                return "";
            }

            string orderBy = " ORDER BY ";

            while (!orderClauses.empty()) {
                orderBy += orderClauses.front();
                orderClauses.pop_front();

                if (!orderClauses.empty()) {
                    orderBy += ", ";
                }
            }

            return orderBy;
        }

        const string getLimitClause() {
            if (rowLimit == NULL_ROW_LIMIT) {
                return "";
            }

            char limitString[16];
            snprintf(limitString, 16, " LIMIT %d", rowLimit);

            return string(limitString);
        }

        const string getStatementCriteria() {
            string criteria =
                getWhereClause() + 
                getOrderBy() + 
                getLimitClause() + 
                ';';

            return criteria;
        }
};

class DBEntity {
    private:
        pfm_id_t insert();
        void update();

        uint64_t findSingleQuotePos(string & s, int startingPos);

    protected:
        virtual void beforeSave() {
            return;
        }

        virtual void afterSave() {
            return;
        }
        
        virtual void beforeInsert() {
            return;
        }

        virtual void afterInsert() {
            return;
        }

        virtual void beforeUpdate() {
            return;
        }

        virtual void afterUpdate() {
            return;
        }

        virtual void beforeRemove() {
            return;
        }

        virtual void afterRemove() {
            return;
        }

        const string getFromClause() {
            string from = "FROM ";
            from.append(getTableName());

            return from;
        }

        string delimitSingleQuotes(string & s);

        struct Columns {
            static constexpr const char * id = "id";
            static constexpr const char * createdDate = "created";
            static constexpr const char * updatedDate = "updated";
        };
        
    public:
        pfm_id_t id;

        string createdDate;
        string updatedDate;
        uint32_t sequence;           // Not persistent

        DBEntity() {
            clear();
        }

        DBEntity(const DBEntity & src) {
            set(src);
        }
        
        virtual ~DBEntity() {}

        virtual JRecord getRecord() {
            JRecord r;
            return r;
        }
        
        virtual const char * getTableName() {
            return "";
        }

        virtual const char * getClassName() {
            return "DBEntity";
        }

        virtual const string getSelectStatement() {
            string statement = "SELECT * " + getFromClause();
            return statement;
        }

        virtual const string getSelectByIDStatement(pfm_id_t & key) {
            DBCriteria criteria;
            criteria.add(Columns::id, DBCriteria::equal_to, key);

            string statement = getSelectStatement() + criteria.getStatementCriteria();

            return statement;
        }

        virtual const string getSelectAllStatement() {
            string statement = getSelectStatement() + ';';
            return statement;
        }

        virtual const string getDeleteByIDStatement(pfm_id_t & key) {
            DBCriteria criteria;
            criteria.add(Columns::id, DBCriteria::equal_to, key);

            string statement = "DELETE " + getFromClause() + criteria.getStatementCriteria();

            return statement;
        }

        virtual const string getDeleteAllStatement() {
            return getDeleteStatement();
        }
        
        virtual const string getInsertStatement() {
            return "";
        }

        virtual const string getUpdateStatement() {
            return "";
        }

        virtual const string getDeleteStatement() {
            string statement = "DELETE " + getFromClause() + ';';

            return statement;
        }

        void remove();
        void remove(const string & statement);
        void removeAll();

        void save();
        void retrieve();
        void retrieve(pfm_id_t & id);

        void clear() {
            this->id.clear();
            this->sequence = 0;
            this->createdDate = "";
            this->updatedDate = "";
        }

        void set(const DBEntity & src) {
            this->id = src.id;
            this->createdDate = src.createdDate;
            this->updatedDate = src.updatedDate;
            this->sequence = src.sequence;
        }

        virtual void assignColumn(DBColumn & column) {
            if (column.getName() == Columns::id) {
                id = column.getIDValue();
            }
            else if (column.getName() == Columns::createdDate) {
                createdDate = column.getValue();
            }
            else if (column.getName() == Columns::updatedDate) {
                updatedDate = column.getValue();
            }
        }

        virtual void onRowComplete(int sequence) {
            return;
        }

        void print() {
            cout << "ID: " << id.getValue() << endl;
            cout << "Sequence: " << sequence << endl;

            cout << "Created: " << createdDate << endl;
            cout << "Updated: " << updatedDate << endl;
        }

        static bool isYesNoBooleanValid(string & ynValue) {
            if (ynValue == "Y" || ynValue == "N") {
                return true;
            }

            return false;
        }
};

class Result {
    private:
        int numRows;
        int sequenceCounter;

    public:
        Result() {
            clear();
        }

        virtual void clear() {
            numRows = 0;
            sequenceCounter = 1;
        }

        int size() {
            return numRows;
        }

        void incrementNumRows() {
            numRows++;
        }
        
        void incrementSequence() {
            sequenceCounter++;
        }

        int getSequence() {
            return sequenceCounter;
        }
};

template <class T>
class DBResult : public Result {
    private:
        vector<T> results;

    public:
        DBResult() : Result() {
            clear();
        }

        void clear() override {
            Result::clear();
            results.clear();
        }

        void reverse() {
            Logger & log = Logger::getInstance();
            log.entry("DBResult::reverse()");

            list<T> l;

            for (int i = results.size() - 1;i >= 0;i--) {
                l.push_back(results[i]);
            }
            for (int i = 0;i < results.size();i++) {
                results[i] = l.front();
                l.pop_front();
            }

            log.exit("DBResult::reverse()");
        }
        
        int retrieve(const string & sqlStatement);
        int retrieveAll();

        T & at(int i) {
            if (size() > i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "at(): Index out of range: numRows: %d, requested row: %d", size(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        T & operator[](int i) {
            return at(i);
        }

        const T & operator[](int i) const {
            return at(i);
        }

        void addRow(T & entity) {
            results.push_back(entity);
        }

        void processRow(DBRow & row) {
            T entity;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                entity.assignColumn(column);
            }
            
            entity.onRowComplete(getSequence());
            incrementSequence();

            addRow(entity);
            incrementNumRows();
        }
};

template <class T>
int DBResult<T>::retrieveAll() {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieveAll()");

    T entity;
    vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(entity.getSelectAllStatement(), &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieveAll()");

    return rowsRetrievedCount;
}

template <class T>
int DBResult<T>::retrieve(const string & sqlStatement) {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieve()");

    vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(sqlStatement, &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieve()");

    return rowsRetrievedCount;
}

#endif
