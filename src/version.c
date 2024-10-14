#include "version.h"

#define __BDATE__      "2024-10-14 19:19:20"
#define __BVERSION__   "0.8.035"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
