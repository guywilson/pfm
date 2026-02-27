#include "version.h"

#define __BDATE__      "2026-02-27 19:11:03"
#define __BVERSION__   "2.1.074"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
