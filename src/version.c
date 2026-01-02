#include "version.h"

#define __BDATE__      "2026-01-02 16:49:48"
#define __BVERSION__   "2.1.027"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
