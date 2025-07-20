#include "version.h"

#define __BDATE__      "2025-07-20 12:21:09"
#define __BVERSION__   "1.3.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
