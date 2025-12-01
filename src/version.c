#include "version.h"

#define __BDATE__      "2025-12-01 15:11:27"
#define __BVERSION__   "2.0.040"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
