#include "version.h"

#define __BDATE__      "2026-02-09 14:56:21"
#define __BVERSION__   "2.1.061"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
