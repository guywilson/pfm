#include "version.h"

#define __BDATE__      "2025-07-31 16:53:10"
#define __BVERSION__   "1.6.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
