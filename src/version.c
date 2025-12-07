#include "version.h"

#define __BDATE__      "2025-12-07 17:19:19"
#define __BVERSION__   "2.0.047"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
