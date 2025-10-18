#include "version.h"

#define __BDATE__      "2025-10-18 13:35:31"
#define __BVERSION__   "1.9.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
