#include "version.h"

#define __BDATE__      "2026-08-29 09:52:26"
#define __BVERSION__   "2.3.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
