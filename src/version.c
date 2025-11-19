#include "version.h"

#define __BDATE__      "2025-11-19 20:12:58"
#define __BVERSION__   "2.0.023"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
