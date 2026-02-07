#include "version.h"

#define __BDATE__      "2026-02-07 19:20:28"
#define __BVERSION__   "2.1.058"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
