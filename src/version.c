#include "version.h"

#define __BDATE__      "2025-12-11 21:34:33"
#define __BVERSION__   "2.0.052"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
