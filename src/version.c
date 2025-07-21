#include "version.h"

#define __BDATE__      "2025-07-21 16:08:26"
#define __BVERSION__   "1.3.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
