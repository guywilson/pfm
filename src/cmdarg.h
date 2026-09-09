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
 
