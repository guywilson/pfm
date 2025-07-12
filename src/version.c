#include "version.h"

#define __BDATE__      "2025-07-12 11:10:07"
#define __BVERSION__   "1.2.019"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
