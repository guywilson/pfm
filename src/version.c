#include "version.h"

#define __BDATE__      "2024-09-21 16:32:56"
#define __BVERSION__   "0.2.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
