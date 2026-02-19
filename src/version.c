#include "version.h"

#define __BDATE__      "2026-02-19 22:36:44"
#define __BVERSION__   "2.1.069"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
