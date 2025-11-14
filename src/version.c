#include "version.h"

#define __BDATE__      "2025-11-14 15:08:45"
#define __BVERSION__   "2.0.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
