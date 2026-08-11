#include "version.h"

#define __BDATE__      "2026-08-11 08:47:51"
#define __BVERSION__   "2.2.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
