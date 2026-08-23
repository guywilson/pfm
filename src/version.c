#include "version.h"

#define __BDATE__      "2026-08-23 17:45:23"
#define __BVERSION__   "2.3.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
