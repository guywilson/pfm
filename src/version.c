#include "version.h"

#define __BDATE__      "2026-01-02 17:17:12"
#define __BVERSION__   "2.1.028"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
