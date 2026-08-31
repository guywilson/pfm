#include "version.h"

#define __BDATE__      "2026-08-31 18:06:11"
#define __BVERSION__   "2.4.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
