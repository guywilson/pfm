#include "version.h"

#define __BDATE__      "2025-08-08 21:48:11"
#define __BVERSION__   "1.8.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
