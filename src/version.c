#include "version.h"

#define __BDATE__      "2024-10-14 19:22:49"
#define __BVERSION__   "0.8.036"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
