#include "version.h"

#define __BDATE__      "2024-09-28 09:59:46"
#define __BVERSION__   "0.5.003"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
