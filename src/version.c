#include "version.h"

#define __BDATE__      "2024-10-02 22:46:40"
#define __BVERSION__   "0.8.014"

const char * getVersion(void) {
    return __BVERSION__;
}

const char * getBuildDate(void) {
    return __BDATE__;
}
