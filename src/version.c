#include "version.h"

#define __BDATE__      "2026-02-03 10:26:45"
#define __BVERSION__   "2.1.048"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
