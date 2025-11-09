#include "version.h"

#define __BDATE__      "2025-11-09 20:00:16"
#define __BVERSION__   "2.0.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
