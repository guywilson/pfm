#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include <string.h>

#include "cmdarg.h"

CmdArg::CmdArg(int argc, char ** argv) {
    for (int i = 1;i < argc;i++) {
        args.push_back(argv[i]);
    }

    argPointer = 0;
}

CmdArg::~CmdArg() {
    args.clear();
}

void CmdArg::dumpArgs() {
    std::cout << "Args:" << std::endl;
    for (size_t i = 0;i < args.size();i++) {
        std::cout << "\t'" << args[i] << "'" << std::endl;
    }
    std::cout << std::endl;
}

std::string CmdArg::nextArg() {
    if (argPointer >= getNumArgs()) {
        throw std::exception();
    }

    return args[argPointer++];
}

std::string CmdArg::getArg(int i) {
    if (i >= getNumArgs()) {
        throw std::exception();
    }

    return args[i];
}
