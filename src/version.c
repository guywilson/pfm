#include "version.h"

#define __BDATE__      "2026-08-22 22:18:46"
#define __BVERSION__   "2.3.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
