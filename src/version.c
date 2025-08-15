#include "version.h"

#define __BDATE__      "2025-08-15 12:17:46"
#define __BVERSION__   "1.8.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
