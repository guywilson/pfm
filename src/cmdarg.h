#pragma once

#include <string>
#include <vector>

#include <stdbool.h>

class CmdArg {
    private:
        int argPointer;
        std::vector<std::string> args;

    public:
        CmdArg(int argc, char ** argv);
        ~CmdArg();

        void dumpArgs();

        inline int getNumArgs() {
            return args.size();
        }

        inline bool hasMoreArgs() {
            return argPointer < getNumArgs();
        }

        inline bool isLastArg() {
            return argPointer == getNumArgs();
        }

        std::string nextArg();
        std::string getArg(int i);
};
 
