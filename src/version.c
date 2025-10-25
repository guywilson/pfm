#include "version.h"

#define __BDATE__      "2025-10-25 20:17:26"
#define __BVERSION__   "1.9.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
