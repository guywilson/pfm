#include "version.h"

#define __BDATE__      "2024-09-16 11:49:21"
#define __BVERSION__   "0.1.006"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
