#include "version.h"

#define __BDATE__      "2024-09-29 21:50:43"
#define __BVERSION__   "0.8.009"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
