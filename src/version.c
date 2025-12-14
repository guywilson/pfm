#include "version.h"

#define __BDATE__      "2025-12-14 09:20:29"
#define __BVERSION__   "2.1.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
