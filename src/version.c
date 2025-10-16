#include "version.h"

#define __BDATE__      "2025-10-16 22:05:34"
#define __BVERSION__   "1.8.044"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
