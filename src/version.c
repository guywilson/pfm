#include "version.h"

#define __BDATE__      "2026-02-03 10:33:52"
#define __BVERSION__   "2.1.049"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
