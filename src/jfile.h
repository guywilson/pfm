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

        object_t getObject();
        void add(const char * name, const string & value);
};

class JFileReader {
    private:
        const char * className;
        json j;

        void validate();

    public:
        JFileReader(string & filename, const char * className);

        vector<JRecord> read(const char * name);
};

class JFileWriter {
    private:
        ofstream fstream;
        const char * className;

    public:
        JFileWriter(string & filename, const char * className);
        JFileWriter(const string & filename, const char * className);
        ~JFileWriter();

        void write(vector<JRecord> & records, const char * name);
        void write(vector<JRecord> & records, string & name);
};

#endif
