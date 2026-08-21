#include "version.h"

#define __BDATE__      "2026-08-21 10:55:23"
#define __BVERSION__   "2.3.005"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
