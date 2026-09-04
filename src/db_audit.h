#pragma once

#include <string>

#include "db_base.h"
#include "db.h"
#include "strdate.h"

#define DB_AUDIT_SQL_STATEMENT_PRINT_WIDTH 80U

int auditOnWriteHandler(const std::string & operation, const std::string & entityName, const std::string & statement);

class DBAuditInteraction : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * auditTimestamp = "audit_timestamp";
            static constexpr ColumnType auditTimestamp_type = ColumnType::TEXT;

            static constexpr const char * entityName = "entity_name";
            static constexpr ColumnType entityName_type = ColumnType::TEXT;

            static constexpr const char * sqlOperation = "sql_operation";
            static constexpr ColumnType sqlOperation_type = ColumnType::TEXT;

            static constexpr const char * sqlStatement = "sql_statement";
            static constexpr ColumnType sqlStatement_type = ColumnType::TEXT;
        };

    public:
        std::string auditTimestamp;
        std::string entityName;
        std::string sqlOperation;
        std::string sqlStatement;

        DBAuditInteraction() : DBEntity() {
            clear();
        }

        DBAuditInteraction(const DBAuditInteraction & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->auditTimestamp.clear();
            this->entityName.clear();
            this->sqlOperation.clear();
            this->sqlStatement.clear();
        }

        void set(const DBAuditInteraction & src) {
            DBEntity::set(src);

            this->auditTimestamp = src.auditTimestamp;
            this->entityName = src.entityName;
            this->sqlOperation = src.sqlOperation;
            this->sqlStatement = src.sqlStatement;
        }

        void print() {
            std::cout << "AuditTimestamp: '" << auditTimestamp << "'" << std::endl;
            std::cout << "EntityName: '" << entityName << "'" << std::endl;
            std::cout << "SQLOperation: '" << sqlOperation << "'" << std::endl;

            static const std::string whitespace = " \t\r\n\f\v";
            static const std::string continuationIndent(sizeof("SQLStatement: '") - 1, ' ');

            std::cout << "SQLStatement: '";

            size_t lineStart = 0;
            bool firstLine = true;

            while (lineStart < sqlStatement.length()) {
                if (!firstLine) {
                    std::cout << std::endl << continuationIndent;
                }

                size_t remaining = sqlStatement.length() - lineStart;
                if (remaining <= DB_AUDIT_SQL_STATEMENT_PRINT_WIDTH) {
                    std::cout << sqlStatement.substr(lineStart);
                    break;
                }

                size_t wrapAt = sqlStatement.find_last_of(
                    whitespace,
                    lineStart + DB_AUDIT_SQL_STATEMENT_PRINT_WIDTH);

                if (wrapAt == std::string::npos || wrapAt < lineStart) {
                    wrapAt = sqlStatement.find_first_of(
                        whitespace,
                        lineStart + DB_AUDIT_SQL_STATEMENT_PRINT_WIDTH);
                }

                if (wrapAt == std::string::npos) {
                    std::cout << sqlStatement.substr(lineStart);
                    break;
                }

                std::cout << sqlStatement.substr(lineStart, wrapAt - lineStart);

                lineStart = sqlStatement.find_first_not_of(whitespace, wrapAt);
                if (lineStart == std::string::npos) {
                    break;
                }

                firstLine = false;
            }

            std::cout << "'" << std::endl;
        }

        const std::string getTableName() const override {
            return "audit_interaction";
        }

        const std::string getClassName() const override {
            return "DBAuditInteraction";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::auditTimestamp, Columns::auditTimestamp_type}, auditTimestamp},
                {{Columns::entityName, Columns::entityName_type}, entityName},
                {{Columns::sqlOperation, Columns::sqlOperation_type}, sqlOperation},
                {{Columns::sqlStatement, Columns::sqlStatement_type}, delimitSingleQuotes(sqlStatement)}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::auditTimestamp, Columns::auditTimestamp_type}, auditTimestamp},
                {{Columns::entityName, Columns::entityName_type}, entityName},
                {{Columns::sqlOperation, Columns::sqlOperation_type}, sqlOperation},
                {{Columns::sqlStatement, Columns::sqlStatement_type}, delimitSingleQuotes(sqlStatement)}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void backup(std::ofstream & os) override {
            DBResult<DBAuditInteraction> results;
            results.retrieveAll();

            os << getDeleteAllStatement() << std::endl;

            for (size_t i = 0;i < results.size();i++) {
                os << results[i].getInsertStatement() << std::endl;
            }

            os.flush();
        }

        void onRowComplete(int sequence) override {
            this->sequence = sequence;
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::auditTimestamp) {
                auditTimestamp = column.getValue();
            }
            else if (column.getName() == Columns::entityName) {
                entityName = column.getValue();
            }
            else if (column.getName() == Columns::sqlOperation) {
                sqlOperation = column.getValue();
            }
            else if (column.getName() == Columns::sqlStatement) {
                sqlStatement = column.getValue();
            }
        }

        DBResult<DBAuditInteraction> retrieveByDateRange(const StrDate & startDate, const StrDate & endDate);
        
        void deleteBeforeDate(const StrDate & date);
};
