#include "version.h"

#define __BDATE__      "2025-11-16 09:32:15"
#define __BVERSION__   "2.0.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
