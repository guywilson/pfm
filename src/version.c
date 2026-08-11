#include "version.h"

#define __BDATE__      "2026-08-11 09:01:50"
#define __BVERSION__   "2.2.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
