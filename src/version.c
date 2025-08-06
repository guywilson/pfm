#include "version.h"

#define __BDATE__      "2025-08-07 00:32:09"
#define __BVERSION__   "1.8.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
