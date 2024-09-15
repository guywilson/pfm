#include "version.h"

#define __BDATE__      "2024-09-15 21:04:17"
#define __BVERSION__   "0.1.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
