#include "version.h"

#define __BDATE__      "2025-11-07 19:58:43"
#define __BVERSION__   "1.9.033"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
