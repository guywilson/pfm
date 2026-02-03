#include "version.h"

#define __BDATE__      "2026-02-03 08:38:38"
#define __BVERSION__   "2.1.044"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
