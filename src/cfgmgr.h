#include <string>
#include <unordered_map>
#include <vector>
#include <exception>

#include <limits.h>
#include <stdint.h>
#include <stdarg.h>

using namespace std;

#ifndef _INCL_CONFIGMGR
#define _INCL_CONFIGMGR

class cfgmgr {
    public:
        static cfgmgr & getInstance() {
            static cfgmgr instance;
            return instance;
        }

    private:
        unordered_map<string, string> values;
        bool isConfigured = false;

        cfgmgr() {}

    public:
        ~cfgmgr() {}

        void clear();
        void initialise();

        string getValue(const string & key);
        bool getValueAsBoolean(const string & key);
        int getValueAsInteger(const string & key);
        int32_t getValueAsLongInteger(const string & key);
        uint32_t getValueAsLongUnsignedInteger(const string & key);
        double getValueAsDouble(const string & key);

        void dumpConfig();
};

#endif
