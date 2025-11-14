#include "version.h"

#define __BDATE__      "2025-11-14 12:04:23"
#define __BVERSION__   "2.0.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
