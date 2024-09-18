#include "version.h"

#define __BDATE__      "2024-09-18 18:28:47"
#define __BVERSION__   "0.1.007"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
