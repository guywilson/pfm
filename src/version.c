#include "version.h"

#define __BDATE__      "2025-12-14 13:48:43"
#define __BVERSION__   "2.1.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
