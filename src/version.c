#include "version.h"

#define __BDATE__      "2025-10-19 22:08:07"
#define __BVERSION__   "1.9.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
