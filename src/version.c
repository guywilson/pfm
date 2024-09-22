#include "version.h"

#define __BDATE__      "2024-09-22 15:55:27"
#define __BVERSION__   "0.3.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
