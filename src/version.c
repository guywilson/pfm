#include "version.h"

#define __BDATE__      "2026-02-07 17:20:30"
#define __BVERSION__   "2.1.057"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
