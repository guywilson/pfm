#include "version.h"

#define __BDATE__      "2026-08-29 10:05:09"
#define __BVERSION__   "2.4.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
