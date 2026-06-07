#include "version.h"

#define __BDATE__      "2026-06-07 08:21:50"
#define __BVERSION__   "2.1.089"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
