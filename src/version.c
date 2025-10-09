#include "version.h"

#define __BDATE__      "2025-10-09 21:39:05"
#define __BVERSION__   "1.8.047"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
