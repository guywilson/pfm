#include "version.h"

#define __BDATE__      "2025-11-29 23:16:04"
#define __BVERSION__   "2.0.034"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
