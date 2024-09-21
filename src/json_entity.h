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
        JRecord(object_t & o);

        string get(const char * name);
};

class JFile {
    private:
        const char * className;
        json j;

        vector<JRecord> records;
        int numRecords;

        void validate();

    public:
        JFile(string & filename, const char * className);

        void readRecords(const char * name);

        int getNumRecords();
        JRecord getRecordAt(int i);
};

#endif
