#include "version.h"

#define __BDATE__      "2024-09-28 21:42:34"
#define __BVERSION__   "0.8.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
