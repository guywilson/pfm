#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <exception>

#include <limits.h>
#include <stdint.h>
#include <stdarg.h>


class cfgmgr {
    public:
        static cfgmgr & getInstance() {
            static cfgmgr instance;
            return instance;
        }

    private:
        std::unordered_map<std::string, std::string> values;
        bool isConfigured = false;

        cfgmgr() {}

    public:
        ~cfgmgr() {}

        void clear();
        void initialise();

        std::string getValue(const std::string & key);
        bool getValueAsBoolean(const std::string & key);
        int getValueAsInteger(const std::string & key);
        int32_t getValueAsLongInteger(const std::string & key);
        uint32_t getValueAsLongUnsignedInteger(const std::string & key);
        double getValueAsDouble(const std::string & key);

        void dumpConfig();
};
