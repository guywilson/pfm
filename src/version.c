#include "version.h"

#define __BDATE__      "2025-12-15 08:44:03"
#define __BVERSION__   "2.1.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
