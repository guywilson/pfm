#include "version.h"

#define __BDATE__      "2025-10-19 11:47:49"
#define __BVERSION__   "1.9.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
