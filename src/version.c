#include "version.h"

#define __BDATE__      "2025-09-02 21:41:30"
#define __BVERSION__   "1.8.028"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
