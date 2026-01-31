#include "version.h"

#define __BDATE__      "2026-01-31 18:53:37"
#define __BVERSION__   "2.1.041"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
