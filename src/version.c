#include "version.h"

#define __BDATE__      "2024-09-09 14:44:10"
#define __BVERSION__   "0.1.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
