#include "version.h"

#define __BDATE__      "2025-07-21 11:53:52"
#define __BVERSION__   "1.3.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
