#include "version.h"

#define __BDATE__      "2025-07-31 16:33:30"
#define __BVERSION__   "1.6.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
