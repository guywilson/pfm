#include "version.h"

#define __BDATE__      "2025-12-06 22:24:55"
#define __BVERSION__   "2.0.046"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
