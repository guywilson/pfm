#include "version.h"

#define __BDATE__      "2025-07-29 16:53:51"
#define __BVERSION__   "1.5.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
