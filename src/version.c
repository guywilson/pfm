#include "version.h"

#define __BDATE__      "2025-08-07 00:47:24"
#define __BVERSION__   "1.8.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
