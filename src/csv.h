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

#include <cstddef>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <vector>

/*
** Mapping file format:
**
** {
**   "hasHeader": true,
**   "delimiter": ",",
**   "skipBlankLines": true,
**   "columns": [
**     { "name": "Date",        "type": "date",   "nullable": false },
**     { "name": "Description", "type": "string" },
**     { "name": "Amount",      "type": "money",  "nullable": false }
**   ]
** }
**
** Supported types are string, integer, double, boolean, date and money.
** Values are retained as strings; the type is metadata for the consumer.
** If the CSV has a header, columns may alternatively be a JSON object whose
** keys are column names and whose values are type names.
*/

class CSV {
    public:
        enum class DataType {
            String,
            Integer,
            Double,
            Boolean,
            Date,
            Money
        };

        struct Column {
            std::string name;
            DataType type = DataType::String;
            std::string value;
            bool nullable = true;
            bool trim = false;
        };

        using Row = std::map<std::string, Column>;

        std::string csvSourceFile;
        
        CSV(const std::string & csvFilename, const std::string & mappingFilename);

        bool hasMoreRows();

        /* Returns an empty map once the input file has been exhausted. */
        Row readRow();

        std::size_t rowNumber() const noexcept;

    private:
        std::ifstream csvStream;

        std::vector<Column> columns;

        char delimiter = ',';
        bool hasHeader = true;
        bool skipBlankLines = true;

        std::size_t physicalLine = 1;
        std::size_t currentRecordLine = 1;
        std::size_t currentRow = 0;
        std::optional<std::vector<std::string>> bufferedFields;
        std::size_t bufferedRecordLine = 1;

        void loadMapping(const std::string & mappingFilename);
        void readAndValidateHeader();
        bool readRecord(std::vector<std::string> & fields);
        bool readNextDataRecord(std::vector<std::string> & fields);
};
