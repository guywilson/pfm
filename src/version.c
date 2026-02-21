#include "version.h"

#define __BDATE__      "2026-02-21 11:22:09"
#define __BVERSION__   "2.1.071"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
