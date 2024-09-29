#include "version.h"

#define __BDATE__      "2024-09-29 21:42:39"
#define __BVERSION__   "0.8.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
