#include "version.h"

#define __BDATE__      "2025-09-01 21:13:49"
#define __BVERSION__   "1.8.025"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
