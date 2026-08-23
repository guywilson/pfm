#include "version.h"

#define __BDATE__      "2026-08-23 18:09:49"
#define __BVERSION__   "2.3.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
