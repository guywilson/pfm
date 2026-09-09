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

#include "../pfm_error.h"

namespace cli::validation {
    inline std::string makeUpperCase(std::string s) {
        for (int i = 0;i < (int)s.length();i++) {
            s[i] = (char)toupper(s.at(i));
        }

        return s;
    }

    inline bool doesExistInString(const std::string & src, const std::string check) {
        std::string upper = makeUpperCase(src);

        if (upper.find(check) == std::string::npos) {
            return false;
        }

        return true;
    }

    inline bool containsInsert(const std::string & fieldValue) {
        return doesExistInString(fieldValue, "INSERT INTO ");
    }

    inline bool containsUpdate(const std::string & fieldValue) {
        return doesExistInString(fieldValue, " UPDATE ");
    }

    inline bool containsDelete(const std::string & fieldValue) {
        return doesExistInString(fieldValue, "DELETE FROM ");
    }

    inline bool containsJoin(const std::string & fieldValue) {
        return doesExistInString(fieldValue, " JOIN ");
    }

    inline bool containsOr(const std::string & fieldValue) {
        return doesExistInString(fieldValue, " OR ");
    }

    inline bool containsDrop(const std::string & fieldValue) {
        return doesExistInString(fieldValue, " DROP ");
    }

    inline bool containsAlter(const std::string & fieldValue) {
        return doesExistInString(fieldValue, " ALTER ");
    }

    inline void validateField(const std::string & fieldValue) {
        if (containsInsert(fieldValue)  || 
            containsUpdate(fieldValue)  || 
            containsDelete(fieldValue)  || 
            containsJoin(fieldValue)    || 
            containsOr(fieldValue)      ||
            containsDrop(fieldValue)    ||
            containsAlter(fieldValue))
        {
            throw pfm_error("Invalid field value, SQL keywords are not permitted");
        }
    }
}
