#include "version.h"

#define __BDATE__      "2024-10-11 10:20:30"
#define __BVERSION__   "0.8.026"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
