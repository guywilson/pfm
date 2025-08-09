#include "version.h"

#define __BDATE__      "2025-08-09 16:03:18"
#define __BVERSION__   "1.8.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
