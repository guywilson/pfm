#include "version.h"

#define __BDATE__      "2025-12-08 11:44:19"
#define __BVERSION__   "2.0.048"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
