#include "version.h"

#define __BDATE__      "2026-01-06 15:01:24"
#define __BVERSION__   "2.1.037"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
