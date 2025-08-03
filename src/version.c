#include "version.h"

#define __BDATE__      "2025-08-03 17:20:11"
#define __BVERSION__   "1.7.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
