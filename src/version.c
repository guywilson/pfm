#include "version.h"

#define __BDATE__      "2025-11-13 17:08:57"
#define __BVERSION__   "2.0.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
