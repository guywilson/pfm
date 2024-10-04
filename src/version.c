#include "version.h"

#define __BDATE__      "2024-10-04 11:20:43"
#define __BVERSION__   "0.8.016"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
