#include "version.h"

#define __BDATE__      "2025-10-18 09:56:14"
#define __BVERSION__   "1.9.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
