#include "version.h"

#define __BDATE__      "2025-07-29 21:03:43"
#define __BVERSION__   "1.6.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
