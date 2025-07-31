#include "version.h"

#define __BDATE__      "2025-07-31 10:07:00"
#define __BVERSION__   "1.6.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
