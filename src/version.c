#include "version.h"

#define __BDATE__      "2025-11-16 17:16:35"
#define __BVERSION__   "2.0.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
