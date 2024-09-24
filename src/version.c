#include "version.h"

#define __BDATE__      "2024-09-24 10:28:02"
#define __BVERSION__   "0.5.001"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
