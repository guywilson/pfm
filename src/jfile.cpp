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

std::vector<json> JFileReader::readJson(const std::string & name) {
    std::vector<json> records = j.at(name).get<std::vector<json>>();

    // Older exports stored money and boolean fields as strings.
    for (json & record : records) {
        for (const char * key : {"amount", "balance", "openingBalance", "balanceLimit"}) {
            if (record.contains(key) && record[key].is_string()) {
                Money amount;
                amount = record[key].get<std::string>();
                record[key] = amount.doubleValue();
            }
        }
        for (const char * key : {"isTransfer", "isReconciled", "isReadOnly", "isVisible"}) {
            if (record.contains(key) && record[key].is_string()) {
                std::string value = record[key].get<std::string>();
                record[key] = (value == "Y" || value == "y");
            }
        }
    }

    return records;
}

void JFileWriter::write(std::vector<json> & records, const std::string & name) {
    write(records, name, this->className);
}

void JFileWriter::write(std::vector<json> & records, const std::string & name, const std::string & className) {
    json entity;
    entity["className"] = className;
    entity[name] = records;
    this->fstream << entity.dump(4) << std::endl;
}
