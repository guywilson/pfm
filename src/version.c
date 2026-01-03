#include "version.h"

#define __BDATE__      "2026-01-03 16:04:43"
#define __BVERSION__   "2.1.030"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
