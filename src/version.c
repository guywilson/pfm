#include "version.h"

#define __BDATE__      "2026-03-22 10:55:21"
#define __BVERSION__   "2.1.083"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
