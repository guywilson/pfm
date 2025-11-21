#include "version.h"

#define __BDATE__      "2025-11-21 08:44:48"
#define __BVERSION__   "2.0.026"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
