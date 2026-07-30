#include "version.h"

#define __BDATE__      "2026-07-30 23:01:34"
#define __BVERSION__   "2.2.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
