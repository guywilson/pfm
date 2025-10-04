#include "version.h"

#define __BDATE__      "2025-10-04 21:51:03"
#define __BVERSION__   "1.8.042"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
