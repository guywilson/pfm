#include "version.h"

#define __BDATE__      "2025-09-02 21:15:41"
#define __BVERSION__   "1.8.027"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
