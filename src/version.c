#include "version.h"

#define __BDATE__      "2025-07-30 11:42:01"
#define __BVERSION__   "1.6.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
