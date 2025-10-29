#include "version.h"

#define __BDATE__      "2025-10-29 21:07:05"
#define __BVERSION__   "1.9.025"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
