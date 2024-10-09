#include "version.h"

#define __BDATE__      "2024-10-09 22:12:45"
#define __BVERSION__   "0.8.022"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
