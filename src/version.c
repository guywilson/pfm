#include "version.h"

#define __BDATE__      "2026-02-09 13:49:49"
#define __BVERSION__   "2.1.059"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
