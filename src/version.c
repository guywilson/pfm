#include "version.h"

#define __BDATE__      "2026-03-07 12:42:53"
#define __BVERSION__   "2.1.080"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
