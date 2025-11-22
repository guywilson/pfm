#include "version.h"

#define __BDATE__      "2025-11-22 15:44:25"
#define __BVERSION__   "2.0.027"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
