#include "version.h"

#define __BDATE__      "2026-08-26 11:11:32"
#define __BVERSION__   "2.3.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
