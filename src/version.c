#include "version.h"

#define __BDATE__      "2026-03-07 15:36:31"
#define __BVERSION__   "2.1.081"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
