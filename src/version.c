#include "version.h"

#define __BDATE__      "2025-10-18 09:46:03"
#define __BVERSION__   "1.9.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
