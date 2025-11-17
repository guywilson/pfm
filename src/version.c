#include "version.h"

#define __BDATE__      "2025-11-17 10:53:38"
#define __BVERSION__   "2.0.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
