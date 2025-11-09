#include "version.h"

#define __BDATE__      "2025-11-09 17:32:01"
#define __BVERSION__   "2.0.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
