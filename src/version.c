#include "version.h"

#define __BDATE__      "2025-07-21 15:24:12"
#define __BVERSION__   "1.3.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
