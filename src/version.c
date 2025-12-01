#include "version.h"

#define __BDATE__      "2025-12-01 10:18:44"
#define __BVERSION__   "2.0.038"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
