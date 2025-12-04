#include "version.h"

#define __BDATE__      "2025-12-04 10:45:36"
#define __BVERSION__   "2.0.041"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
