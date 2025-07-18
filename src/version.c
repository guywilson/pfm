#include "version.h"

#define __BDATE__      "2025-07-18 16:12:14"
#define __BVERSION__   "1.2.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
