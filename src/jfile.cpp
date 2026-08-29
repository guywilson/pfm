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
#include "jfile.h"

using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

JRecord::JRecord() {
}

JRecord::JRecord(object_t & o) {
    this->record = o;
}

std::string JRecord::get(const char * name) {
    if (record.count(name) == 0) {
        throw pfm_validation_error(
                        pfm_error::buildMsg(
                            "Json record does not contain field '%s'", 
                            name));
    }

    return record[name];
}

bool JRecord::getBoolValue(const char * name) {
    std::string value = get(name);

    return (value == "Y" ? true : false);
}

object_t JRecord::getObject() {
    return this->record;
}

void JRecord::add(const char * name, const std::string & value) {
    record[name] = value;
}

void JRecord::add(const char * name, const bool value) {
    record[name] = (value ? "Y" : "N");
}

void JFileReader::validate(const std::string & className) {
    std::unordered_map<std::string, json> elements = j.template get<std::unordered_map<std::string, json>>();

    bool foundClassName = false;
    std::string fileClassName;

    for (auto& i : elements) {
        if (i.first.compare("className") == 0) {
            fileClassName = i.second;

            if (fileClassName == className) {
                foundClassName = true;
            }
            break;
        }
    }

    if (!foundClassName) {
        throw pfm_validation_error(
                    pfm_error::buildMsg(
                        "Error importing categories, invalid className '%s', expected '%s'", 
                        fileClassName.c_str(),
                        className.c_str()));
    }
}

JFileReader::JFileReader(const std::string & filename) {
    std::ifstream fstream(filename);
    this->j = json::parse(fstream);
    fstream.close();
}

std::vector<JRecord> JFileReader::read(const std::string & name) {
    std::vector<JRecord> records;

    objects_t rows = j.at(name).get<objects_t>();

    for (object_t & row : rows) {
        JRecord record = JRecord(row);
        records.push_back(record);
    }

    return records;
}

JFileWriter::JFileWriter(const std::string & filename) {
    this->fstream.open(filename);
}

JFileWriter::JFileWriter(const std::string & filename, const std::string & className) {
    this->className = className;
    this->fstream.open(filename);
}

JFileWriter::~JFileWriter() {
    this->fstream.close();
}

void JFileWriter::write(std::vector<JRecord> & records, const std::string & name) {
    auto jsonEntities = json::array();

    for (JRecord record : records) {
        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["className"] = this->className;
    entity[name] = {jsonEntities};

    this->fstream << entity.dump(4) << std::endl;
}

void JFileWriter::write(std::vector<JRecord> & records, const std::string & name, const std::string & className) {
    auto jsonEntities = json::array();

    for (JRecord record : records) {
        json j = json::object();
        object_t o = record.getObject();

        for (const auto& [key, value] : o) {
            j[key] = value;
        }

        jsonEntities.push_back(j);
    }

    json entity;
    entity["className"] = className;
    entity[name] = {jsonEntities};

    this->fstream << entity.dump(4) << std::endl;
}
