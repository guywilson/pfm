#include "version.h"

#define __BDATE__      "2026-08-26 20:53:48"
#define __BVERSION__   "2.3.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
