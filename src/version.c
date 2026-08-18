#include "version.h"

#define __BDATE__      "2026-08-18 18:34:00"
#define __BVERSION__   "2.3.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
