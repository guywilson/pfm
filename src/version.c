#include "version.h"

#define __BDATE__      "2026-01-06 12:23:53"
#define __BVERSION__   "2.1.036"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
