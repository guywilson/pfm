#include "version.h"

#define __BDATE__      "2026-08-24 09:46:38"
#define __BVERSION__   "2.3.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
