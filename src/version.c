#include "version.h"

#define __BDATE__      "2024-09-28 23:13:49"
#define __BVERSION__   "0.8.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
