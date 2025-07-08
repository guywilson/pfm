#include "version.h"

#define __BDATE__      "2025-07-08 22:14:26"
#define __BVERSION__   "1.2.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
