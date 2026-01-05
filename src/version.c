#include "version.h"

#define __BDATE__      "2026-01-05 07:48:52"
#define __BVERSION__   "2.1.034"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
