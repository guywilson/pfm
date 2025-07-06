#include "version.h"

#define __BDATE__      "2025-07-06 15:44:54"
#define __BVERSION__   "1.2.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
