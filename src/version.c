#include "version.h"

#define __BDATE__      "2025-07-06 17:16:24"
#define __BVERSION__   "1.2.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
