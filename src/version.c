#include "version.h"

#define __BDATE__      "2025-12-14 10:10:27"
#define __BVERSION__   "2.1.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
