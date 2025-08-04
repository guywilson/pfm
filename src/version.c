#include "version.h"

#define __BDATE__      "2025-08-04 23:12:31"
#define __BVERSION__   "1.8.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
