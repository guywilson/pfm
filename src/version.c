#include "version.h"

#define __BDATE__      "2025-09-21 16:11:18"
#define __BVERSION__   "1.8.038"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
