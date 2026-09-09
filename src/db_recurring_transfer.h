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
#include "db_account.h"
#include "strdate.h"


class DBRecurringTransfer : public DBEntity {
    protected:
        struct Columns {
            static constexpr const char * recurringChargeId = "recurring_charge_id";
            static constexpr ColumnType recurringChargeId_type = ColumnType::ID;

            static constexpr const char * accountToId = "account_to_id";
            static constexpr ColumnType accountToId_type = ColumnType::ID;
        };

    public:
        DBAccount accountTo;
        
        pfm_id_t recurringChargeId;
        pfm_id_t accountToId;

        DBRecurringTransfer() : DBEntity() {
            clear();
        }

        DBRecurringTransfer(const DBRecurringTransfer & src) : DBEntity(src) {
            set(src);
        }

        void clear() {
            DBEntity::clear();

            this->recurringChargeId.clear();
            this->accountToId.clear();
        }

        void set(const DBRecurringTransfer & src) {
            DBEntity::set(src);

            this->accountTo = src.accountTo;
            this->recurringChargeId = src.recurringChargeId;
            this->accountToId = src.accountToId;
        }

        void print() {
            DBEntity::print();

            std::cout << "RecurringChargeId: " << recurringChargeId.getValue() << std::endl;
            std::cout << "AccountToId: " << accountToId.getValue() << std::endl;
        }

        void onRowComplete(int sequence) override {
            if (!accountToId.isNull()) {
                accountTo.retrieve(accountToId);
            }
        }

        bool inline isNull() {
            return (id.isNull() && accountToId.isNull() && recurringChargeId.isNull());
        }

        const std::string getTableName() const override {
            return "recurring_transfer";
        }

        const std::string getClassName() const override {
            return "DBRecurringTransfer";
        }

        const std::string getInsertStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::accountToId, Columns::accountToId_type}, accountToId.getValue()}
            };

            return buildInsertStatement(getTableName(), columnValuePairs);
        }

        const std::string getUpdateStatement() override {
            std::vector<std::pair<ColumnDef, std::string>> columnValuePairs = {
                {{Columns::recurringChargeId, Columns::recurringChargeId_type}, recurringChargeId.getValue()},
                {{Columns::accountToId, Columns::accountToId_type}, accountToId.getValue()}
            };

            return buildUpdateStatement(getTableName(), columnValuePairs);
        }

        void assignColumn(DBColumn & column) override {
            DBEntity::assignColumn(column);
            
            if (column.getName() == Columns::recurringChargeId) {
                recurringChargeId = column.getIDValue();
            }
            else if (column.getName() == Columns::accountToId) {
                accountToId = column.getIDValue();
            }
        }

        int retrieveByRecurringChargeId(pfm_id_t & recurringChargeId);
};
