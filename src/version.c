#include "version.h"

#define __BDATE__      "2025-08-01 21:22:56"
#define __BVERSION__   "1.6.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
