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
