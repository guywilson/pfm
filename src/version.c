#include "version.h"

#define __BDATE__      "2026-02-22 16:55:26"
#define __BVERSION__   "2.1.073"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
