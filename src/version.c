#include "version.h"

#define __BDATE__      "2025-11-09 20:05:57"
#define __BVERSION__   "2.0.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
