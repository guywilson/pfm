#include "version.h"

#define __BDATE__      "2026-06-03 17:01:09"
#define __BVERSION__   "2.1.086"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
