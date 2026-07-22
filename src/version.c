#include "version.h"

#define __BDATE__      "2026-07-22 22:16:51"
#define __BVERSION__   "2.1.096"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
