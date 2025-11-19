#include "version.h"

#define __BDATE__      "2025-11-19 12:54:44"
#define __BVERSION__   "2.0.021"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
