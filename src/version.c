#include "version.h"

#define __BDATE__      "2025-12-01 14:09:29"
#define __BVERSION__   "2.0.039"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
