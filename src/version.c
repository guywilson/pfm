#include "version.h"

#define __BDATE__      "2025-12-31 11:27:33"
#define __BVERSION__   "2.1.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
