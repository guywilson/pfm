#include "version.h"

#define __BDATE__      "2025-07-21 21:23:18"
#define __BVERSION__   "1.4.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
