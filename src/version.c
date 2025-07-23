#include "version.h"

#define __BDATE__      "2025-07-23 15:41:11"
#define __BVERSION__   "1.5.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
