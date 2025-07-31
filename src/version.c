#include "version.h"

#define __BDATE__      "2025-07-31 16:43:40"
#define __BVERSION__   "1.6.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
