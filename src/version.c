#include "version.h"

#define __BDATE__      "2026-01-04 09:07:34"
#define __BVERSION__   "2.1.032"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
