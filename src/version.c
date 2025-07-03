#include "version.h"

#define __BDATE__      "2025-07-03 21:45:21"
#define __BVERSION__   "1.1.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
