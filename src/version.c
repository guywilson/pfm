#include "version.h"

#define __BDATE__      "2025-11-27 09:37:45"
#define __BVERSION__   "2.0.033"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
