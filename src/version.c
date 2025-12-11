#include "version.h"

#define __BDATE__      "2025-12-11 13:36:09"
#define __BVERSION__   "2.0.051"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
