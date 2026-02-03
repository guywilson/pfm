#include "version.h"

#define __BDATE__      "2026-02-03 09:46:05"
#define __BVERSION__   "2.1.047"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
