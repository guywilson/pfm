#include "version.h"

#define __BDATE__      "2025-07-04 21:48:01"
#define __BVERSION__   "1.2.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
