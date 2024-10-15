#include "version.h"

#define __BDATE__      "2024-10-15 13:43:30"
#define __BVERSION__   "0.8.038"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
