#include "version.h"

#define __BDATE__      "2026-05-17 15:14:22"
#define __BVERSION__   "2.1.085"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
