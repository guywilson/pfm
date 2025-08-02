#include "version.h"

#define __BDATE__      "2025-08-02 17:59:20"
#define __BVERSION__   "1.7.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
