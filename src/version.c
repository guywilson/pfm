#include "version.h"

#define __BDATE__      "2026-01-01 20:37:33"
#define __BVERSION__   "2.1.023"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
