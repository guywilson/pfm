#include "version.h"

#define __BDATE__      "2025-09-13 11:35:44"
#define __BVERSION__   "1.8.031"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
