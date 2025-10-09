#include "version.h"

#define __BDATE__      "2025-10-04 21:59:20"
#define __BVERSION__   "1.8.043"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
