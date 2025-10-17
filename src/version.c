#include "version.h"

#define __BDATE__      "2025-10-17 11:50:05"
#define __BVERSION__   "1.8.046"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
