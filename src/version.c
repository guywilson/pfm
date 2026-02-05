#include "version.h"

#define __BDATE__      "2026-02-05 13:16:34"
#define __BVERSION__   "2.1.056"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
