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
#include <unordered_map>
#include <vector>
#include <exception>

#include <limits.h>
#include <stdint.h>
#include <stdarg.h>


class cfgmgr {
    public:
        static cfgmgr & getInstance() {
            static cfgmgr instance;
            return instance;
        }

    private:
        std::unordered_map<std::string, std::string> values;
        bool isConfigured = false;

        cfgmgr() {}

    public:
        ~cfgmgr() {}

        void clear();
        void initialise();

        std::string getValue(const std::string & key);
        bool getValueAsBoolean(const std::string & key);
        int getValueAsInteger(const std::string & key);
        int32_t getValueAsLongInteger(const std::string & key);
        uint32_t getValueAsLongUnsignedInteger(const std::string & key);
        double getValueAsDouble(const std::string & key);

        void dumpConfig();
};
