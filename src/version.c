#include "version.h"

#define __BDATE__      "2026-08-19 08:47:11"
#define __BVERSION__   "2.3.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
