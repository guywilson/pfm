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

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "money.h"
#include "strdate.h"
#include "pfm_error.h"

using json = nlohmann::json;

class JFileReader {
    private:
        json j;

    public:
        JFileReader(const std::string & filename);

        std::vector<json> readJson(const std::string & name);

        void validate(const std::string & className);
};

class JFileWriter {
    private:
        std::ofstream fstream;
        std::string className;

    public:
        JFileWriter(const std::string & filename);
        JFileWriter(const std::string & filename, const std::string & className);
        ~JFileWriter();

        void write(std::vector<json> & records, const std::string & name);
        void write(std::vector<json> & records, const std::string & name, const std::string & className);
};
