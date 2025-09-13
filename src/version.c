#include "version.h"

#define __BDATE__      "2025-09-13 21:20:46"
#define __BVERSION__   "1.8.036"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
