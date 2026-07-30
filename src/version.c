#include "version.h"

#define __BDATE__      "2026-07-30 22:51:21"
#define __BVERSION__   "2.2.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
