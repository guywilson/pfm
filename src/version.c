#include "version.h"

#define __BDATE__      "2025-07-25 21:44:59"
#define __BVERSION__   "1.5.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
