#include "version.h"

#define __BDATE__      "2025-11-10 15:31:07"
#define __BVERSION__   "2.0.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
