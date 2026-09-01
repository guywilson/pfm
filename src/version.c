#include "version.h"

#define __BDATE__      "2026-09-01 16:06:28"
#define __BVERSION__   "2.4.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
