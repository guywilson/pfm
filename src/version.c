#include "version.h"

#define __BDATE__      "2026-01-02 13:11:24"
#define __BVERSION__   "2.1.025"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
