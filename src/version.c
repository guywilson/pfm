#include "version.h"

#define __BDATE__      "2025-11-06 13:17:31"
#define __BVERSION__   "1.9.030"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
