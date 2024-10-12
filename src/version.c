#include "version.h"

#define __BDATE__      "2024-10-12 10:17:30"
#define __BVERSION__   "0.8.030"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
