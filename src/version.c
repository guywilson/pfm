#include "version.h"

#define __BDATE__      "2024-09-28 10:33:07"
#define __BVERSION__   "0.5.004"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
