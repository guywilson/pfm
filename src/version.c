#include "version.h"

#define __BDATE__      "2025-11-07 09:05:48"
#define __BVERSION__   "1.9.031"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
