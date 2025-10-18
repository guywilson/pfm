#include "version.h"

#define __BDATE__      "2025-10-18 23:23:31"
#define __BVERSION__   "1.9.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
