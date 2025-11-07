#include "version.h"

#define __BDATE__      "2025-11-07 11:52:51"
#define __BVERSION__   "1.9.032"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
