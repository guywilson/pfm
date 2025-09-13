#include "version.h"

#define __BDATE__      "2025-09-13 11:28:47"
#define __BVERSION__   "1.8.030"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
