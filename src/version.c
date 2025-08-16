#include "version.h"

#define __BDATE__      "2025-08-16 09:12:03"
#define __BVERSION__   "1.8.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
