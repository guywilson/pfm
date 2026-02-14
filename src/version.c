#include "version.h"

#define __BDATE__      "2026-02-14 20:22:26"
#define __BVERSION__   "2.1.063"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
