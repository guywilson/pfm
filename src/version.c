#include "version.h"

#define __BDATE__      "2026-02-05 11:28:54"
#define __BVERSION__   "2.1.054"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
