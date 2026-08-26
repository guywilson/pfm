#include "version.h"

#define __BDATE__      "2026-08-26 07:30:43"
#define __BVERSION__   "2.3.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
