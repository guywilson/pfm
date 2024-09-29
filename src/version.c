#include "version.h"

#define __BDATE__      "2024-09-29 21:46:19"
#define __BVERSION__   "0.8.008"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
