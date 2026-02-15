#include "version.h"

#define __BDATE__      "2026-02-15 21:11:49"
#define __BVERSION__   "2.1.066"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
