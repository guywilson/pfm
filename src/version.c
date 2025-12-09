#include "version.h"

#define __BDATE__      "2025-12-09 21:33:38"
#define __BVERSION__   "2.0.050"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
