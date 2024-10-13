#include "version.h"

#define __BDATE__      "2024-10-13 12:30:19"
#define __BVERSION__   "0.8.034"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
