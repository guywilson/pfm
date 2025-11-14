#include "version.h"

#define __BDATE__      "2025-11-14 15:26:10"
#define __BVERSION__   "2.0.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
