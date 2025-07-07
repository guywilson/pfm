#include "version.h"

#define __BDATE__      "2025-07-07 15:49:22"
#define __BVERSION__   "1.2.014"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
