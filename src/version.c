#include "version.h"

#define __BDATE__      "2024-10-09 22:20:10"
#define __BVERSION__   "0.8.023"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
