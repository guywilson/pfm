#include "version.h"

#define __BDATE__      "2025-11-16 07:59:30"
#define __BVERSION__   "2.0.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
