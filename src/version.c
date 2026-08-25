#include "version.h"

#define __BDATE__      "2026-08-25 22:23:37"
#define __BVERSION__   "2.3.014"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
