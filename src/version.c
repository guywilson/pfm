#include "version.h"

#define __BDATE__      "2026-06-08 08:05:24"
#define __BVERSION__   "2.1.090"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
