#include "version.h"

#define __BDATE__      "2024-09-22 21:17:39"
#define __BVERSION__   "0.3.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
