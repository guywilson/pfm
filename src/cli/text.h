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

namespace cli::text {
    inline int countMultiByteChars(const std::string & str) {
        int i = 0;
        for (unsigned char c : str) {
            if (c & 0x80) {
                i++;
            }
        }

        return i;
    }

    inline int calculateFieldWidth(const std::string & value, int baseWidth) {
        /*
        ** Handle 2-byte characters, e.g. currency symbols,
        ** width is adjusted here for such strings.
        ** Changes specified in P2675 should fix this:
        ** https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2675r1.pdf
        */
        int width = baseWidth;
        int numMultiByteChars = countMultiByteChars(value);

        if (numMultiByteChars > 0) {
            if (numMultiByteChars >= 2) {
                numMultiByteChars--;
            }

            width += numMultiByteChars;
        }

        return width;
    }
}
