#include "version.h"

#define __BDATE__      "2025-12-14 14:58:30"
#define __BVERSION__   "2.1.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
