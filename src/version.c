#include "version.h"

#define __BDATE__      "2024-10-09 08:37:29"
#define __BVERSION__   "0.8.020"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
