#include "version.h"

#define __BDATE__      "2026-01-08 16:58:37"
#define __BVERSION__   "2.1.038"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
