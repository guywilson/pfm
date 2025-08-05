#include "version.h"

#define __BDATE__      "2025-08-05 10:07:41"
#define __BVERSION__   "1.8.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
