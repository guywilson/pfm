#include "version.h"

#define __BDATE__      "2026-08-31 21:01:52"
#define __BVERSION__   "2.4.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
