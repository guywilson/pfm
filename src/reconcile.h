#pragma once

#include <istream>
#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include <cstring>

#define CSV_MAX_LINE_LENGTH         256

using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

class TransactionReconciler {
    public:
        void reconcileTransactions(std::string & bankCSVName, std::string & csvMappingName) {
            std::ifstream mappingFile(csvMappingName);
            json j = json::parse(mappingFile);
            mappingFile.close();

            std::ifstream csv(bankCSVName);

            char csvLine[CSV_MAX_LINE_LENGTH];

            while (!csv.eof()) {
                csv.getline(csvLine, CSV_MAX_LINE_LENGTH);

            }
        }
};
