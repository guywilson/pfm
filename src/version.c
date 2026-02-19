#include "version.h"

#define __BDATE__      "2026-02-19 17:58:08"
#define __BVERSION__   "2.1.068"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
