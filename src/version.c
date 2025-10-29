#include "version.h"

#define __BDATE__      "2025-10-29 21:14:25"
#define __BVERSION__   "1.9.026"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
