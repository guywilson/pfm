#include "version.h"

#define __BDATE__      "2025-11-13 15:41:27"
#define __BVERSION__   "2.0.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
