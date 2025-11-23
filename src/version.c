#include "version.h"

#define __BDATE__      "2025-11-23 16:40:38"
#define __BVERSION__   "2.0.028"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
