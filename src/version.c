#include "version.h"

#define __BDATE__      "2025-12-18 09:26:40"
#define __BVERSION__   "2.1.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
