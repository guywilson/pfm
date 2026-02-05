#include "version.h"

#define __BDATE__      "2026-02-05 13:05:49"
#define __BVERSION__   "2.1.055"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
