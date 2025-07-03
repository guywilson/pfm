#include "version.h"

#define __BDATE__      "2025-07-03 11:48:11"
#define __BVERSION__   "1.1.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
