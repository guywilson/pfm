#include "version.h"

#define __BDATE__      "2025-07-05 16:33:22"
#define __BVERSION__   "1.2.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
