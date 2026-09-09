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
#include <stdint.h>

#include <sqlcipher/sqlite3.h>

#include "db_carried_over.h"
#include "db.h"
#include "db_base.h"
#include "strdate.h"


class DBCarriedOverView : public DBCarriedOver {
    protected:
        struct Columns {
            static constexpr const char * accountCode = "account_code";
            static constexpr ColumnType accountCode_type = ColumnType::TEXT;
        };

    public:
        std::string accountCode;

        DBCarriedOverView() : DBCarriedOver() {
            clear();
        }

        DBCarriedOverView(const DBCarriedOverView & src) : DBCarriedOver(src) {
            set(src);
        }

        void clear() {
            DBCarriedOver::clear();

            this->accountCode = "";
        }

        void set(const DBCarriedOverView & src) {
            DBCarriedOver::set(src);

            this->accountCode = src.accountCode;
        }

        void assignColumn(DBColumn & column) override {
            DBCarriedOver::assignColumn(column);
            
            if (column.getName() == Columns::accountCode) {
                accountCode = column.getValue();
            }
        }

        const std::string getTableName() const override {
            return "v_carried_over_log";
        }

        const std::string getClassName() const override {
            return "DBCarriedOverView";
        }
};
