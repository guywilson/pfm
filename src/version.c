#include "version.h"

#define __BDATE__      "2024-10-05 21:33:51"
#define __BVERSION__   "0.8.017"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
