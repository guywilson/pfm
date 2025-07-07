#include "version.h"

#define __BDATE__      "2025-07-07 09:28:12"
#define __BVERSION__   "1.2.012"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
