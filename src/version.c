#include "version.h"

#define __BDATE__      "2025-07-28 17:59:46"
#define __BVERSION__   "1.5.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
