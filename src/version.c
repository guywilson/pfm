#include "version.h"

#define __BDATE__      "2025-10-05 10:05:56"
#define __BVERSION__   "1.8.045"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
