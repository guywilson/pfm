#include "version.h"

#define __BDATE__      "2024-09-28 22:13:31"
#define __BVERSION__   "0.8.002"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
