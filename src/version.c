#include "version.h"

#define __BDATE__      "2025-11-26 21:35:04"
#define __BVERSION__   "2.0.031"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
