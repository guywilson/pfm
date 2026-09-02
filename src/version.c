#include "version.h"

#define __BDATE__      "2026-09-02 21:08:14"
#define __BVERSION__   "2.4.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
