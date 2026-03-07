#pragma once

#include <string>
#include <vector>

#include <stdbool.h>

using namespace std;

class CmdArg {
    private:
        int argPointer;
        vector<string> args;

    public:
        CmdArg(int argc, char ** argv);
        ~CmdArg();

        int getNumArgs();
        bool hasMoreArgs();
        string nextArg();

        string getArg(int i);
};
 
