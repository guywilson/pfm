#include "version.h"

#define __BDATE__      "2024-10-12 18:08:20"
#define __BVERSION__   "0.8.031"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
