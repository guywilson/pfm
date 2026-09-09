#include "version.h"

#define __BDATE__      "2026-09-09 17:42:00"
#define __BVERSION__   "2.4.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
