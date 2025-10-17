#include "version.h"

#define __BDATE__      "2025-10-17 11:25:42"
#define __BVERSION__   "1.8.045"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
