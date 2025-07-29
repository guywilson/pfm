#include "version.h"

#define __BDATE__      "2025-07-29 21:00:52"
#define __BVERSION__   "1.5.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
