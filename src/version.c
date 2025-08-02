#include "version.h"

#define __BDATE__      "2025-08-02 12:04:35"
#define __BVERSION__   "1.6.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
