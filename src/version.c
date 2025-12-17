#include "version.h"

#define __BDATE__      "2025-12-17 11:43:47"
#define __BVERSION__   "2.1.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
