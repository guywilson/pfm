#include "version.h"

#define __BDATE__      "2026-07-21 21:17:16"
#define __BVERSION__   "2.1.093"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
