#include "version.h"

#define __BDATE__      "2024-10-13 10:47:14"
#define __BVERSION__   "0.8.032"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
