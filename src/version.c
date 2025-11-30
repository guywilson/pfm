#include "version.h"

#define __BDATE__      "2025-11-30 09:55:22"
#define __BVERSION__   "2.0.036"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
