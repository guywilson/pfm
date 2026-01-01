#include "version.h"

#define __BDATE__      "2026-01-01 17:19:30"
#define __BVERSION__   "2.1.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
