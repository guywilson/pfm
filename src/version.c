#include "version.h"

#define __BDATE__      "2025-12-08 13:25:47"
#define __BVERSION__   "2.0.049"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
