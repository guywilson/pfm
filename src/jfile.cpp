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

using namespace std;
using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

JRecord::JRecord() {
}

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

object_t JRecord::getObject() {
    return this->record;
}

void JRecord::add(const char * name, const string & value) {
    record[name] = value;
}

void JFileReader::validate() {
    unordered_map<string, json> elements = j.template get<unordered_map<string, json>>();

    for (auto& i : elements) {
        if (i.first.compare("className") == 0) {
            string fileClassName = i.second;

            if (fileClassName.compare(className) != 0) {
                throw pfm_validation_error(
                            pfm_error::buildMsg(
                                "Error importing categories, invalid className '%s', expected '%s'", 
                                fileClassName.c_str(),
                                className.c_str()));
            }

            break;
        }
    }
}

JFileReader::JFileReader(const string & filename, const string & className) {
    this->className = className;

    ifstream fstream(filename.c_str());
    this->j = json::parse(fstream);
    fstream.close();

    validate();
}

vector<JRecord> JFileReader::read(const char * name) {
    vector<JRecord> records;

    objects_t rows = j.at(name).get<objects_t>();

    for (object_t & row : rows) {
        JRecord record = JRecord(row);
        records.push_back(record);
    }

    return records;
}

JFileWriter::JFileWriter(const string & filename, const string & className) {
    this->className = className;
    this->fstream.open(filename);
}

JFileWriter::~JFileWriter() {
    this->fstream.close();
}

void JFileWriter::write(vector<JRecord> & records, const char * name) {
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

    this->fstream << entity.dump(4) << endl;
}

void JFileWriter::write(vector<JRecord> & records, const string & name) {
    write(records, name.c_str());
}
