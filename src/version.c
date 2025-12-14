#include "version.h"

#define __BDATE__      "2025-12-14 22:23:48"
#define __BVERSION__   "2.1.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
