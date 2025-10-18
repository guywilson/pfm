#include "version.h"

#define __BDATE__      "2025-10-18 23:02:41"
#define __BVERSION__   "1.9.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
