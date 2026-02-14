#include "version.h"

#define __BDATE__      "2026-02-14 20:47:05"
#define __BVERSION__   "2.1.064"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
