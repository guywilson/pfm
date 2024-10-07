#include "version.h"

#define __BDATE__      "2024-10-07 14:37:54"
#define __BVERSION__   "0.8.018"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
