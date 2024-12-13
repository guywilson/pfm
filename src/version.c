#include "version.h"

#define __BDATE__      "2024-12-13 23:58:06"
#define __BVERSION__   "1.0.044"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
