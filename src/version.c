#include "version.h"

#define __BDATE__      "2024-10-08 10:29:30"
#define __BVERSION__   "0.8.019"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
