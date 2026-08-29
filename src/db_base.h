#pragma once

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


#define SQL_STATEMENT_LENGTH                   256
#define LIMIT_CLAUSE_BUFFER_LEN                 32
#define SINGLE_QUOTE_CHAR                       39
#define NULL_ROW_LIMIT                          -1

template <class T> class DBResult;

class DBCriteria {
    private:
        std::deque<std::string> whereClauses;
        std::deque<std::string> orderClauses;
        std::deque<std::string> groupClauses;
        std::multimap<std::string, std::string> inClauses;
        int rowLimit;

        std::string appendString(std::string & clause, const std::string & value) {
            clause.append("'");
            clause.append(value);
            clause.append("'");

            return clause;
        }

        std::string appendBoolean(std::string & clause, const bool value) {
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
            groupClauses.clear();
            inClauses.clear();
            rowLimit = NULL_ROW_LIMIT;
        }

        void add(const std::string & columnName, enum sql_operator op) {
            std::string clause = columnName;

            switch (op) {
                case is_null:
                    clause.append(" IS NULL");
                    break;

                case is_not_null:
                    clause.append(" IS NOT NULL");
                    break;

                default:
                    throw pfm_error("DBCriteria::add() - Illegal operation, only is_null or is_not_null expected");
            }

            whereClauses.push_back(clause);
        }

        void add(const std::string & columnName, enum sql_operator op, const std::string & value) {
            std::string clause = columnName;

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

        void addIgnoreSQ(const std::string & columnName, enum sql_operator op, const std::string & value) {
            std::string clause = columnName;

            switch (op) {
                case less_than:
                    clause.append(" < ");
                    clause.append(value);
                    break;

                case less_than_or_equal:
                    clause.append(" <= ");
                    clause.append(value);
                    break;

                case greater_than:
                    clause.append(" > ");
                    clause.append(value);
                    break;

                case greater_than_or_equal:
                    clause.append(" >= ");
                    clause.append(value);
                    break;

                case equal_to:
                    clause.append(" = ");
                    clause.append(value);
                    break;

                case not_equal_to:
                    clause.append(" != ");
                    clause.append(value);
                    break;

                case like:
                    clause.append(" LIKE ");
                    clause.append(value);
                    break;

                case not_like:
                    clause.append(" NOT LIKE ");
                    clause.append(value);
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

        void add(const std::string & columnName, enum sql_operator op, const StrDate & value) {
            add(columnName, op, value.shortDate());
        }

        void add(const std::string & columnName, enum sql_operator op, const Money & value) {
            std::string clause = columnName;

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

        void add(const std::string & columnName, enum sql_operator op, const pfm_id_t & id) {
            std::string clause = columnName;

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

        void addFirst(const std::string & columnName, enum sql_operator op, const pfm_id_t & id) {
            std::string clause = columnName;

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

        void add(const std::string & columnName, const bool value) {
            std::string clause = columnName;

            clause.append(" = ");
            clause = appendBoolean(clause, value);

            whereClauses.push_back(clause);
        }

        void addToInClause(const std::string & columnName, const std::string & value) {
            inClauses.insert(std::pair<std::string, std::string>{columnName, value});
        }

        void addToInClause(const std::string & columnName, const StrDate & value) {
            addToInClause(columnName, value.shortDate());
        }

        void addToInClause(const std::string & columnName, const pfm_id_t & value) {
            addToInClause(columnName, value.getValue());
        }

        void endInClause_string(const std::string & columnName) {
            std::string clause = columnName + " IN (";

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

        void endInClause_StrDate(const std::string & columnName) {
            endInClause_string(columnName);
        }

        void endInClause_id(const std::string & columnName) {
            std::string clause = columnName + " IN (";

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

        void addOrderBy(const std::string & columnName, enum sql_order order) {
            std::string clause = columnName + ' ';

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

        void addGroupBy(const std::string & columnName) {
            groupClauses.push_back(columnName);
        }

        void setRowLimit(int numRows) {
            rowLimit = numRows;
        }

        const std::string getWhereCriteria() const {
            if (whereClauses.empty()) {
                return "";
            }

            std::string where;
            for (size_t i = 0; i < whereClauses.size(); i++) {
                where += whereClauses[i];

                if (i + 1 < whereClauses.size()) {
                    where += " AND ";
                }
            }

            return where;
        }

        const std::string getWhereClause() const {
            return " WHERE " + getWhereCriteria();
        }

        const std::string getOrderBy() const {
            if (orderClauses.empty()) {
                return "";
            }

            std::string orderBy = " ORDER BY ";
            for (size_t i = 0; i < orderClauses.size(); i++) {
                orderBy += orderClauses[i];

                if (i + 1 < orderClauses.size()) {
                    orderBy += ", ";
                }
            }

            return orderBy;
        }

        const std::string getGroupBy() const {
            if (groupClauses.empty()) {
                return "";
            }

            std::string groupBy = " GROUP BY ";
            for (size_t i = 0; i < groupClauses.size(); i++) {
                groupBy += groupClauses[i];

                if (i + 1 < groupClauses.size()) {
                    groupBy += ", ";
                }
            }

            return groupBy;
        }

        const std::string getLimitClause() const {
            if (rowLimit == NULL_ROW_LIMIT) {
                return "";
            }

            char limitString[16];
            snprintf(limitString, 16, " LIMIT %d", rowLimit);

            return std::string(limitString);
        }

        const std::string getStatementCriteria() const {
            std::string criteria =
                getWhereClause() + 
                getOrderBy() + 
                getGroupBy() +
                getLimitClause() + 
                ';';

            return criteria;
        }
};

class DBEntity {
    private:
        pfm_id_t insert();
        void update();

        uint64_t findSingleQuotePos(std::string & s, int startingPos) const;

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

        const std::string getFromClause() {
            std::string from = "FROM ";
            from.append(getTableName());

            return from;
        }

        const std::string delimitSingleQuotes(std::string & s) const;
        
        virtual std::string buildInsertStatement(const std::string & tableName, std::vector<std::pair<ColumnDef, std::string>> & columnValuePairs) const {
            std::string now = StrDate::getTimestamp();

            columnValuePairs.push_back({{Columns::createdDate, Columns::createdDate_type}, now});
            columnValuePairs.push_back({{Columns::updatedDate, Columns::updatedDate_type}, now});

            std::string cols = "(";
            std::string vals = "(";

            for (size_t i = 0;i < columnValuePairs.size();i++) {
                ColumnDef column = columnValuePairs[i].first;
                std::string value = columnValuePairs[i].second;

                cols.append(column.name);

                if (column.type == ID || column.type == MONEY) {
                    vals.append(value);
                }
                else {
                    vals.append("'");
                    vals.append(value);
                    vals.append("'");
                }

                if (i < columnValuePairs.size() - 1) {
                    cols.append(", ");
                    vals.append(", ");
                }
            }

            cols.append(")");
            vals.append(")");

            std::string statement = "INSERT INTO " + tableName + " " + cols + " VALUES " + vals + ";";

            return statement;
        }
        
        virtual std::string buildUpdateStatement(const std::string & tableName, std::vector<std::pair<ColumnDef, std::string>> & columnValuePairs) const {
            std::string now = StrDate::getTimestamp();

            columnValuePairs.push_back({{Columns::updatedDate, Columns::updatedDate_type}, now});
 
            std::string statement = "UPDATE " + tableName + " SET ";

            for (size_t i = 0;i < columnValuePairs.size();i++) {
                ColumnDef column = columnValuePairs[i].first;
                std::string value = columnValuePairs[i].second;

                statement.append(column.name);
                statement.append(" = ");

                if (column.type == ID || column.type == MONEY) {
                    statement.append(value);
                }
                else {
                    statement.append("'");
                    statement.append(value);
                    statement.append("'");
                }

                if (i < columnValuePairs.size() - 1) {
                    statement.append(", ");
                }
            }

            statement.append(" WHERE ");
            statement.append(Columns::id);
            statement.append(" = ");
            statement.append(id.getValue());
            statement.append(";");

            return statement;
        }

        struct Columns {
            static constexpr const char * id = "id";
            static constexpr ColumnType id_type = ID;

            static constexpr const char * createdDate = "created";
            static constexpr ColumnType createdDate_type = TEXT;

            static constexpr const char * updatedDate = "updated";
            static constexpr ColumnType updatedDate_type = TEXT;
        };
        
    public:
        pfm_id_t id;

        std::string createdDate;
        std::string updatedDate;
        
        /*
        ** Not persistent...
        */
        uint32_t sequence;
        bool isSavedBySystem = false;

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

        virtual void backup(std::ofstream & os) {
            return;
        }
        
        virtual const std::string getTableName() const {
            return "";
        }

        virtual const std::string getClassName() const {
            return "DBEntity";
        }

        virtual const std::string getJSONArrayName() const {
            return "";
        }

        virtual const std::string getSelectStatement() {
            std::string statement = "SELECT * " + getFromClause();
            return statement;
        }

        virtual const std::string getSelectByIDStatement(const pfm_id_t & key) {
            DBCriteria criteria;
            criteria.add(Columns::id, DBCriteria::equal_to, key);

            std::string statement = getSelectStatement() + criteria.getStatementCriteria();

            return statement;
        }

        virtual const std::string getSelectAllStatement() {
            std::string statement = getSelectStatement() + ';';
            return statement;
        }

        virtual const std::string getDeleteByIDStatement(pfm_id_t & key) {
            DBCriteria criteria;
            criteria.add(Columns::id, DBCriteria::equal_to, key);

            std::string statement = "DELETE " + getFromClause() + criteria.getStatementCriteria();

            return statement;
        }

        virtual const std::string getDeleteAllStatement() {
            return getDeleteStatement();
        }

        virtual const std::string getInsertStatement() {
            return "";
        }

        virtual const std::string getUpdateStatement() {
            return "";
        }

        virtual const std::string getDeleteStatement() {
            std::string statement = "DELETE " + getFromClause() + ';';

            return statement;
        }

        void remove();
        void remove(const std::string & statement);
        void removeAll();

        void save();
        void retrieve();
        void retrieve(const pfm_id_t & id);

        void clear() {
            this->id.clear();
            this->sequence = 0;
            this->isSavedBySystem = false;
            this->createdDate = "";
            this->updatedDate = "";
        }

        void set(const DBEntity & src) {
            this->id = src.id;
            this->createdDate = src.createdDate;
            this->updatedDate = src.updatedDate;
            this->sequence = src.sequence;
            this->isSavedBySystem = src.isSavedBySystem;
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
            std::cout << "ID: " << id.getValue() << std::endl;
            std::cout << "Sequence: " << sequence << std::endl;

            std::cout << "Created: " << createdDate << std::endl;
            std::cout << "Updated: " << updatedDate << std::endl;
        }

        static bool isYesNoBooleanValid(std::string & ynValue) {
            if (ynValue == "Y" || ynValue == "N") {
                return true;
            }

            return false;
        }
};

class Result {
    private:
        size_t numRows;
        int sequenceCounter;

    public:
        Result() {
            clear();
        }

        virtual void clear() {
            numRows = 0;
            sequenceCounter = 1;
        }

        size_t size() {
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
        std::vector<T> results;

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

            std::list<T> l;

            for (int i = results.size() - 1;i >= 0;i--) {
                l.push_back(results[i]);
            }
            for (size_t i = 0;i < results.size();i++) {
                results[i] = l.front();
                l.pop_front();
            }

            log.exit("DBResult::reverse()");
        }
        
        int retrieve(const std::string & sqlStatement);
        int retrieveAll();

        T & at(unsigned int i) {
            if (size() > (size_t)i) {
                return results[i];
            }
            else {
                throw pfm_error(
                        pfm_error::buildMsg(
                            "at(): Index out of range: numRows: %zu, requested row: %u", size(), i), 
                        __FILE__, 
                        __LINE__);
            }
        }

        T & operator[](unsigned int i) {
            return at(i);
        }

        const T & operator[](unsigned int i) const {
            return at(i);
        }

        void addRow(T & entity) {
            entity.onRowComplete(getSequence());
            incrementSequence();

            results.push_back(entity);
            incrementNumRows();
        }

        void processRow(DBRow & row) {
            T entity;

            for (size_t i = 0;i < row.getNumColumns();i++) {
                DBColumn column = row.getColumnAt(i);

                entity.assignColumn(column);
            }
            
            addRow(entity);
        }
};

template <class T>
int DBResult<T>::retrieveAll() {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieveAll()");

    T entity;
    std::vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(entity.getSelectAllStatement(), &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieveAll()");

    return rowsRetrievedCount;
}

template <class T>
int DBResult<T>::retrieve(const std::string & sqlStatement) {
    Logger & log = Logger::getInstance();
    log.entry("DBResult::retrieve()");

    std::vector<DBRow> rows;

    PFM_DB & db = PFM_DB::getInstance();

    int rowsRetrievedCount = db.executeSelect(sqlStatement, &rows);

    for (int i = 0;i < rowsRetrievedCount;i++) {
        processRow(rows[i]);
    }

    log.exit("DBResult::retrieve()");

    return rowsRetrievedCount;
}
