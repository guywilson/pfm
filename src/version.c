#include "version.h"

#define __BDATE__      "2025-12-06 11:36:49"
#define __BVERSION__   "2.0.042"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
