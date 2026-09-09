/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sqlcipher/sqlite3.h>

#include "pfm_error.h"
#include "db_base.h"
#include "db.h"
#include "db_transaction.h"
#include "strdate.h"
#include "money.h"
#include "cfgmgr.h"


class DBTransferTransactionRecord : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * transactionToId = "transaction_to_id";
            static constexpr ColumnType transactionToId_type = ColumnType::ID;

            static constexpr const char * transactionFromId = "transaction_from_id";
            static constexpr ColumnType transactionFromId_type = ColumnType::ID;

            static constexpr const char * transactionDate = "transaction_date";
            static constexpr ColumnType transactionDate_type = ColumnType::DATE;
        };

    public:
        pfm_id_t transactionToId;
        pfm_id_t transactionFromId;
        StrDate transactionDate;

        DBTransferTransactionRecord() : DBEntity() {
            clear();
        }

        DBTransferTransactionRecord(const DBTransferTransactionRecord & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->transactionFromId.clear();
            this->transactionToId.clear();
            this->transactionDate.clear();
        }

        void set(const DBTransferTransactionRecord & src) {
            DBEntity::set(src);

            this->transactionFromId = src.transactionFromId;
            this->transactionToId = src.transactionToId;
            this->transactionDate = src.transactionDate;
        }

        void print() {
            DBEntity::print();

            std::cout << "TransactionFromId: " << transactionFromId.getValue() << std::endl;
            std::cout << "TransactionToId: " << transactionToId.getValue() << std::endl;
            std::cout << "TransactionDate: " << transactionDate.shortDate() << std::endl;
        }

        const std::string getTableName() const override {
            return "transfer_transaction_record";
        }

        const std::string getClassName() const override {
            return "DBTransferTransactionRecord";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::transactionToId, Columns::transactionToId_type}, transactionToId.getValue()},
                {{Columns::transactionFromId, Columns::transactionFromId_type}, transactionFromId.getValue()},
                {{Columns::transactionDate, Columns::transactionDate_type}, transactionDate.shortDate()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::transactionToId, Columns::transactionToId_type}, transactionToId.getValue()},
                {{Columns::transactionFromId, Columns::transactionFromId_type}, transactionFromId.getValue()},
                {{Columns::transactionDate, Columns::transactionDate_type}, transactionDate.shortDate()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::transactionToId) {
                transactionToId = column.getIDValue();
            }
            else if (column.getName() == Columns::transactionFromId) {
                transactionFromId = column.getIDValue();
            }
            else if (column.getName() == Columns::transactionDate) {
                transactionDate = column.getValue();
            }
        }

        int retrieveByTransactionToId(pfm_id_t & transactionToId);
        int retrieveByTransactionFromId(pfm_id_t & transactionFromId);
        int retrieveByTransactionIds(pfm_id_t & sourceTransactionId, pfm_id_t & targetTransactionId);

        static DBTransferTransactionRecord createFromTransactions(DBTransaction & transactionTo, DBTransaction & transactionFrom);
};
