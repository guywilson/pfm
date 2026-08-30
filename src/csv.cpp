#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "pfm_error.h"
#include "csv.h"

namespace {

using json = nlohmann::json;

void throwValidationError(const std::string & message) {
    throw pfm_validation_error(message.c_str());
}

std::string trim(const std::string & value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });

    if (first == value.end()) {
        return {};
    }

    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();

    return std::string(first, last);
}

std::string lowerCase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

} // namespace

CSV::CSV(const std::string & csvFilename, const std::string & mappingFilename) {
    loadMapping(mappingFilename);

    csvStream.open(csvFilename, std::ios::binary);
    if (!csvStream.is_open()) {
        throwValidationError("Unable to open CSV file '" + csvFilename + "'");
    }

    if (hasHeader) {
        readAndValidateHeader();
    }
}

void CSV::loadMapping(const std::string & mappingFilename) {
    std::ifstream mappingStream(mappingFilename);
    if (!mappingStream.is_open()) {
        throwValidationError("Unable to open CSV mapping file '" + mappingFilename + "'");
    }

    json mapping;
    try {
        mappingStream >> mapping;
    }
    catch (const json::exception & error) {
        throwValidationError(
                "Invalid JSON in CSV mapping file '" + mappingFilename + "': " + error.what());
    }

    try {
        if (mapping.is_object()) {
            hasHeader = mapping.value("hasHeader", true);
            skipBlankLines = mapping.value("skipBlankLines", true);

            if (mapping.contains("delimiter")) {
                const std::string delimiterValue = mapping.at("delimiter").get<std::string>();
                if (delimiterValue.size() != 1 || delimiterValue[0] == '"' ||
                    delimiterValue[0] == '\r' || delimiterValue[0] == '\n') {
                    throwValidationError("CSV mapping delimiter must be one character and cannot be a quote or newline");
                }
                delimiter = delimiterValue[0];
            }
        }

        const json * columnMappings = &mapping;
        if (mapping.is_object() && mapping.contains("columns")) {
            columnMappings = &mapping.at("columns");
        }

        auto parseType = [](const std::string & suppliedType) {
            const std::string type = lowerCase(suppliedType);

            if (type == "string" || type == "text" || type == "std::string") {
                return DataType::String;
            }
            if (type == "integer" || type == "int" || type == "long" ||
                type == "int64" || type == "int64_t" || type == "std::int64_t") {
                return DataType::Integer;
            }
            if (type == "double" || type == "float" || type == "decimal" || type == "number") {
                return DataType::Double;
            }
            if (type == "boolean" || type == "bool") {
                return DataType::Boolean;
            }
            if (type == "date" || type == "strdate") {
                return DataType::Date;
            }
            if (type == "money" || type == "currency") {
                return DataType::Money;
            }

            throwValidationError("Unknown CSV mapping data type '" + suppliedType + "'");
        };

        if (columnMappings->is_array()) {
            for (const json & item : *columnMappings) {
                if (!item.is_object() || !item.contains("name") || !item.contains("type")) {
                    throwValidationError("Each CSV column mapping must contain string fields 'name' and 'type'");
                }

                Column column;
                column.name = item.at("name").get<std::string>();
                column.type = parseType(item.at("type").get<std::string>());
                column.nullable = item.value("nullable", true);
                column.trim = item.value("trim", false);
                columns.push_back(std::move(column));
            }
        }
        else if (columnMappings->is_object()) {
            if (!hasHeader) {
                throwValidationError("Object-form CSV column mappings require hasHeader to be true");
            }

            for (const auto & [name, type] : columnMappings->items()) {
                if (name == "hasHeader" || name == "delimiter" || name == "skipBlankLines") {
                    continue;
                }
                if (!type.is_string()) {
                    throwValidationError("CSV mapping type for column '" + name + "' must be a string");
                }

                Column column;
                column.name = name;
                column.type = parseType(type.get<std::string>());
                columns.push_back(std::move(column));
            }
        }
        else {
            throwValidationError("CSV mapping must be a JSON array or object");
        }
    }
    catch (const json::exception & error) {
        throwValidationError(
                "Invalid CSV mapping file '" + mappingFilename + "': " + error.what());
    }

    if (columns.empty()) {
        throwValidationError("CSV mapping must define at least one column");
    }

    std::unordered_set<std::string> names;
    for (const Column & column : columns) {
        if (column.name.empty()) {
            throwValidationError("CSV mapping column names cannot be empty");
        }
        if (!names.emplace(column.name).second) {
            throwValidationError("Duplicate CSV mapping column '" + column.name + "'");
        }
    }
}

void CSV::readAndValidateHeader() {
    std::vector<std::string> header;
    if (!readRecord(header)) {
        throwValidationError("CSV file is empty; expected a header row");
    }

    if (!header.empty() && header[0].starts_with("\xEF\xBB\xBF")) {
        header[0].erase(0, 3);
    }

    if (header.size() != columns.size()) {
        throwValidationError(
                "CSV header has " + std::to_string(header.size()) + " columns; mapping defines " +
                std::to_string(columns.size()));
    }

    std::unordered_map<std::string, Column> mappedColumns;
    for (Column & column : columns) {
        mappedColumns.emplace(column.name, std::move(column));
    }

    std::vector<Column> orderedColumns;
    orderedColumns.reserve(header.size());
    for (const std::string & name : header) {
        auto column = mappedColumns.find(name);
        if (column == mappedColumns.end()) {
            throwValidationError("CSV header contains unmapped column '" + name + "'");
        }
        orderedColumns.push_back(std::move(column->second));
        mappedColumns.erase(column);
    }

    columns = std::move(orderedColumns);
}

bool CSV::readRecord(std::vector<std::string> & fields) {
    fields.clear();

    std::string field;
    bool inQuotes = false;
    bool quoteClosed = false;
    bool readAnyCharacter = false;
    currentRecordLine = physicalLine;

    char c;
    while (csvStream.get(c)) {
        readAnyCharacter = true;

        if (inQuotes) {
            if (c == '"') {
                if (csvStream.peek() == '"') {
                    csvStream.get(c);
                    field.push_back('"');
                }
                else {
                    inQuotes = false;
                    quoteClosed = true;
                }
            }
            else {
                if (c == '\n') {
                    physicalLine++;
                }
                field.push_back(c);
            }
            continue;
        }

        if (quoteClosed) {
            if (c == delimiter) {
                fields.push_back(std::move(field));
                field.clear();
                quoteClosed = false;
                continue;
            }
            if (c == '\r' || c == '\n') {
                if (c == '\r' && csvStream.peek() == '\n') {
                    csvStream.get(c);
                }
                physicalLine++;
                fields.push_back(std::move(field));
                return true;
            }

            throwValidationError(
                    "Unexpected character after closing quote in CSV record beginning at line " +
                    std::to_string(currentRecordLine));
        }

        if (c == '"') {
            if (!field.empty()) {
                throwValidationError(
                        "Unexpected quote in unquoted CSV field at line " + std::to_string(physicalLine));
            }
            inQuotes = true;
        }
        else if (c == delimiter) {
            fields.push_back(std::move(field));
            field.clear();
        }
        else if (c == '\r' || c == '\n') {
            if (c == '\r' && csvStream.peek() == '\n') {
                csvStream.get(c);
            }
            physicalLine++;
            fields.push_back(std::move(field));
            return true;
        }
        else {
            field.push_back(c);
        }
    }

    if (csvStream.bad()) {
        throwValidationError(
                "I/O error while reading CSV record beginning at line " +
                std::to_string(currentRecordLine));
    }

    if (inQuotes) {
        throwValidationError(
                "Unterminated quoted CSV field in record beginning at line " +
                std::to_string(currentRecordLine));
    }

    if (!readAnyCharacter && field.empty() && fields.empty()) {
        return false;
    }

    fields.push_back(std::move(field));
    return true;
}

CSV::Row CSV::readRow() {
    Row row;
    std::vector<std::string> fields;

    if (bufferedFields.has_value()) {
        fields = std::move(*bufferedFields);
        bufferedFields.reset();
        currentRecordLine = bufferedRecordLine;
    }
    else if (!readNextDataRecord(fields)) {
        return row;
    }

    currentRow++;

    if (fields.size() != columns.size()) {
        throwValidationError(
                "CSV row " + std::to_string(currentRow) + " (line " +
                std::to_string(currentRecordLine) + ") has " + std::to_string(fields.size()) +
                " columns; expected " + std::to_string(columns.size()));
    }

    for (std::size_t index = 0; index < columns.size(); index++) {
        Column column = columns[index];
        column.value = column.trim ? trim(fields[index]) : fields[index];

        if (!column.nullable && column.value.empty()) {
            throwValidationError(
                    "Empty value for non-nullable CSV column '" + column.name + "' at row " +
                    std::to_string(currentRow));
        }

        row.emplace(column.name, std::move(column));
    }

    return row;
}

bool CSV::hasMoreRows() {
    if (bufferedFields.has_value()) {
        return true;
    }

    std::vector<std::string> fields;
    if (!readNextDataRecord(fields)) {
        return false;
    }

    bufferedFields = std::move(fields);
    bufferedRecordLine = currentRecordLine;
    return true;
}

bool CSV::readNextDataRecord(std::vector<std::string> & fields) {
    while (readRecord(fields)) {
        if (skipBlankLines && fields.size() == 1 && fields[0].empty()) {
            continue;
        }

        if (currentRow == 0 && !hasHeader && !fields.empty() &&
            fields[0].starts_with("\xEF\xBB\xBF")) {
            fields[0].erase(0, 3);
        }

        return true;
    }

    return false;
}

std::size_t CSV::rowNumber() const noexcept {
    return currentRow;
}
