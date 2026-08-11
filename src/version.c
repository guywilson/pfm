#include "version.h"

#define __BDATE__      "2026-08-11 21:58:32"
#define __BVERSION__   "2.2.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
