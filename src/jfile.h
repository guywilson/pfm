#pragma once

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

using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

class JRecord {
    private:
        object_t record;

    public:
        JRecord();
        JRecord(object_t & o);

        std::string get(const char * name);
        bool getBoolValue(const char * name);

        object_t getObject();
        void add(const char * name, const std::string & value);
        void add(const char * name, const bool value);
};

class JFileReader {
    private:
        json j;

    public:
        JFileReader(const std::string & filename);

        std::vector<JRecord> read(const std::string & name);

        void validate(const std::string & className);
};

class JFileWriter {
    private:
        std::ofstream fstream;
        std::string className;

    public:
        JFileWriter(const std::string & filename);
        JFileWriter(const std::string & filename, const std::string & className);
        ~JFileWriter();

        void write(std::vector<JRecord> & records, const std::string & name, const std::string & className);
        void write(std::vector<JRecord> & records, const std::string & name);
};
