#include "version.h"

#define __BDATE__      "2026-08-23 22:08:36"
#define __BVERSION__   "2.3.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
