#include "version.h"

#define __BDATE__      "2025-11-30 10:14:20"
#define __BVERSION__   "2.0.037"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
