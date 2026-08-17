#include "version.h"

#define __BDATE__      "2026-08-17 22:45:12"
#define __BVERSION__   "2.3.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
