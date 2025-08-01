#include "version.h"

#define __BDATE__      "2025-08-01 14:52:01"
#define __BVERSION__   "1.6.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
