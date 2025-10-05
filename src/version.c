#include "version.h"

#define __BDATE__      "2025-10-05 08:34:15"
#define __BVERSION__   "1.8.044"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
