#include "version.h"

#define __BDATE__      "2025-11-01 09:33:54"
#define __BVERSION__   "1.9.028"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
