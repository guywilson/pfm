#include "version.h"

#define __BDATE__      "2026-02-15 21:14:23"
#define __BVERSION__   "2.1.067"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
