/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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
