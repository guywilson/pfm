#include "version.h"

#define __BDATE__      "2026-09-04 13:39:42"
#define __BVERSION__   "2.4.014"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
