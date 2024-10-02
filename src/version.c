#include "version.h"

#define __BDATE__      "2024-10-02 09:04:02"
#define __BVERSION__   "0.8.013"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
