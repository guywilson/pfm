#include "version.h"

#define __BDATE__      "2025-07-04 19:39:02"
#define __BVERSION__   "1.2.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
