#include "version.h"

#define __BDATE__      "2024-09-30 21:44:41"
#define __BVERSION__   "0.8.010"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
