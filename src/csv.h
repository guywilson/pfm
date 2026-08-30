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
