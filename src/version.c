#include "version.h"

#define __BDATE__      "2025-11-15 12:39:18"
#define __BVERSION__   "2.0.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
