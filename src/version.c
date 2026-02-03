#include "version.h"

#define __BDATE__      "2026-02-03 08:43:25"
#define __BVERSION__   "2.1.045"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
