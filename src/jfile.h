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

using namespace std;
using json = nlohmann::json;

using object_t = std::map<std::string, std::string>;
using objects_t = std::vector<object_t>;

#ifndef __INCL_JSON_ENTITY
#define __INCL_JSON_ENTITY

class JRecord {
    private:
        object_t record;

    public:
        JRecord();
        JRecord(object_t & o);

        string get(const char * name);
        bool getBoolValue(const char * name);

        object_t getObject();
        void add(const char * name, const string & value);
        void add(const char * name, const bool value);
};

class JFileReader {
    private:
        string className;
        json j;

        void validate();

    public:
        JFileReader(const string & filename, const string & className);

        vector<JRecord> read(const char * name);
};

class JFileWriter {
    private:
        ofstream fstream;
        string className;

    public:
        JFileWriter(const string & filename, const string & className);
        ~JFileWriter();

        void write(vector<JRecord> & records, const char * name);
        void write(vector<JRecord> & records, const string & name);
};

#endif
