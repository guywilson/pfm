#include "version.h"

#define __BDATE__      "2026-08-01 22:19:45"
#define __BVERSION__   "2.2.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
