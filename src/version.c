#include "version.h"

#define __BDATE__      "2025-08-31 10:18:37"
#define __BVERSION__   "1.8.021"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
