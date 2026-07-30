#include "version.h"

#define __BDATE__      "2026-07-30 23:03:03"
#define __BVERSION__   "2.2.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
