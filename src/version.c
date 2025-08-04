#include "version.h"

#define __BDATE__      "2025-08-04 11:05:51"
#define __BVERSION__   "1.8.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
