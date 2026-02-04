#include "version.h"

#define __BDATE__      "2026-02-04 21:50:41"
#define __BVERSION__   "2.1.053"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
