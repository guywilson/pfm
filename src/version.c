#include "version.h"

#define __BDATE__      "2026-06-06 17:58:22"
#define __BVERSION__   "2.1.088"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
