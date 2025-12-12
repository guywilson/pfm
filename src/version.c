#include "version.h"

#define __BDATE__      "2025-12-12 20:06:57"
#define __BVERSION__   "2.1.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
