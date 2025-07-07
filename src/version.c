#include "version.h"

#define __BDATE__      "2025-07-07 19:00:37"
#define __BVERSION__   "1.2.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
