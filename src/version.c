#include "version.h"

#define __BDATE__      "2024-10-10 08:51:37"
#define __BVERSION__   "0.8.025"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
