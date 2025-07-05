#include "version.h"

#define __BDATE__      "2025-07-05 15:40:18"
#define __BVERSION__   "1.2.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
