#include "version.h"

#define __BDATE__      "2025-12-06 15:33:34"
#define __BVERSION__   "2.0.043"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
