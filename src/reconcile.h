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

#include <string>
#include <vector>

#include "db_temp_csv.h"
#include "csv.h"

class TransactionReconciler {
    private:
        void dropCSVTempTable();
        void dropReconciliationView();

        std::vector<DBRow> reportPart1(const std::string & accountCode);
        std::vector<DBRow> reportPart2(const std::string & accountCode, const StrDate & startDate, const StrDate & endDate);
        std::vector<DBRow> reportPart3(const std::string & accountCode, const StrDate & startDate, const StrDate & endDate);

        void populateCSVTempTable(const std::string & accountCode, CSV & csv);

    public:
        void reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName);
};
