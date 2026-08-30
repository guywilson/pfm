#include "version.h"

#define __BDATE__      "2026-08-30 22:33:30"
#define __BVERSION__   "2.4.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
