#include "version.h"

#define __BDATE__      "2025-10-04 13:59:08"
#define __BVERSION__   "1.8.041"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
