#include "version.h"

#define __BDATE__      "2024-10-11 20:14:32"
#define __BVERSION__   "0.8.028"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
