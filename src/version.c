#include "version.h"

#define __BDATE__      "2025-07-31 11:48:06"
#define __BVERSION__   "1.6.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
