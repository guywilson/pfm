#include "version.h"

#define __BDATE__      "2026-01-10 17:09:51"
#define __BVERSION__   "2.1.039"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
