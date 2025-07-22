#include "version.h"

#define __BDATE__      "2025-07-22 15:39:01"
#define __BVERSION__   "1.4.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
