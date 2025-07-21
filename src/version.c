#include "version.h"

#define __BDATE__      "2025-07-21 18:58:44"
#define __BVERSION__   "1.4.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
