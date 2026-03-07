#include "version.h"

#define __BDATE__      "2026-03-07 12:27:16"
#define __BVERSION__   "2.1.079"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
