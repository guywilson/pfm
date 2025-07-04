#include "version.h"

#define __BDATE__      "2025-07-04 15:20:30"
#define __BVERSION__   "1.1.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
