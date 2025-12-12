#include "version.h"

#define __BDATE__      "2025-12-12 14:34:26"
#define __BVERSION__   "2.0.053"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
