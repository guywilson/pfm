#include "version.h"

#define __BDATE__      "2025-11-24 10:49:32"
#define __BVERSION__   "2.0.030"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
