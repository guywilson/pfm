#include "version.h"

#define __BDATE__      "2025-11-14 15:31:39"
#define __BVERSION__   "2.0.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
