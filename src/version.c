#include "version.h"

#define __BDATE__      "2025-12-19 16:39:04"
#define __BVERSION__   "2.1.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
