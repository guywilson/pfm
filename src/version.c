#include "version.h"

#define __BDATE__      "2025-11-20 14:28:07"
#define __BVERSION__   "2.0.025"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
