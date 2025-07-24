#include "version.h"

#define __BDATE__      "2025-07-24 17:49:53"
#define __BVERSION__   "1.5.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
