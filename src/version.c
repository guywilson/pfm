#include "version.h"

#define __BDATE__      "2026-01-01 17:06:04"
#define __BVERSION__   "2.1.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
