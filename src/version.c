#include "version.h"

#define __BDATE__      "2026-01-29 16:43:32"
#define __BVERSION__   "2.1.040"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
