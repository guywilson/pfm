#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "pfm_error.h"
#include "json_entity.h"

using namespace std;
using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

JRecord::JRecord(object_t & o) {
    this->record = o;
}

string JRecord::get(const char * name) {
    if (record.count(name) == 0) {
        throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "Json record does not contain field '%s'", 
                            name));
    }

    return record[name];
}

void JFile::validate() {
    unordered_map<string, json> elements = j.template get<unordered_map<string, json>>();

    for (auto& i : elements) {
        if (i.first.compare("className") == 0) {
            string fileClassName = i.second;

            if (fileClassName.compare(className) != 0) {
                throw pfm_validation_error(
                            pfm_error::buildMsg(
                                "Error importing categories, invalid className '%s', expected '%s'", 
                                fileClassName.c_str(),
                                className));
            }

            break;
        }
    }
}

JFile::JFile(string & filename, const char * className) {
    this->className = className;

    ifstream fstream(filename.c_str());
    this->j = json::parse(fstream);
    fstream.close();

    validate();
}

vector<JRecord> JFile::readRecords(const char * name) {
    vector<JRecord> records;

    objects_t rows = j.at(name).get<objects_t>();

    for (object_t & row : rows) {
        JRecord record = JRecord(row);
        records.push_back(record);
    }

    return records;
}
