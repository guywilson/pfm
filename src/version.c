#include "version.h"

#define __BDATE__      "2024-09-30 21:57:55"
#define __BVERSION__   "0.8.011"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
