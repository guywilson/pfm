#include "version.h"

#define __BDATE__      "2025-07-30 14:35:20"
#define __BVERSION__   "1.6.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
