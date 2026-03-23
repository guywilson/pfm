#include "version.h"

#define __BDATE__      "2026-03-23 08:03:26"
#define __BVERSION__   "2.1.084"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
