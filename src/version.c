#include "version.h"

#define __BDATE__      "2025-11-18 18:18:26"
#define __BVERSION__   "2.0.019"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
