#include "version.h"

#define __BDATE__      "2026-03-06 21:34:01"
#define __BVERSION__   "2.1.078"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
