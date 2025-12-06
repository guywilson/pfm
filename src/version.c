#include "version.h"

#define __BDATE__      "2025-12-06 22:07:08"
#define __BVERSION__   "2.0.044"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
