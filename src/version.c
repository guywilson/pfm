#include "version.h"

#define __BDATE__      "2025-07-22 10:16:04"
#define __BVERSION__   "1.4.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
