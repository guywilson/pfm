#include "version.h"

#define __BDATE__      "2025-10-31 21:52:24"
#define __BVERSION__   "1.9.027"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
