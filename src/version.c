#include "version.h"

#define __BDATE__      "2024-10-13 11:56:38"
#define __BVERSION__   "0.8.033"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
