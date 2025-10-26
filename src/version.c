#include "version.h"

#define __BDATE__      "2025-10-26 18:10:29"
#define __BVERSION__   "1.9.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
