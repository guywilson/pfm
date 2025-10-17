#include "version.h"

#define __BDATE__      "2025-10-17 13:40:45"
#define __BVERSION__   "1.8.049"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
