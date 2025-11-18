#include "version.h"

#define __BDATE__      "2025-11-18 21:10:22"
#define __BVERSION__   "2.0.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
