#include "version.h"

#define __BDATE__      "2025-08-02 10:50:16"
#define __BVERSION__   "1.6.015"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
