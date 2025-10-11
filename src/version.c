#include "version.h"

#define __BDATE__      "2025-10-11 16:39:50"
#define __BVERSION__   "1.8.043"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
