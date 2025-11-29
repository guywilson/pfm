#include "version.h"

#define __BDATE__      "2025-11-29 23:23:19"
#define __BVERSION__   "2.0.035"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
