#pragma once

#include <iostream>

#if defined(__APPLE__) || defined(__unix__)
const char ESC_CHAR = '\x1B';
const char SEQ_OPEN = '[';
const char SEQ_CLOSE = 'm';

/*
cout << set_style(TextStyle::Bold | TextStyle::Underline, Colour::Red) << "Text" << set_style(TextStyle::Reset) << endl;
*/
enum class TextStyle {
    Reset      = 0x00,      // 0
    Bold       = 0x01,      // 1
    Underline  = 0x02,      // 4
    Inverse    = 0x04       // 7
};

enum class Colour {
    Black   = 30,
    Red     = 31,
    Green   = 32,
    Yellow  = 33,
    Blue    = 34,
    Magenta = 35,
    Cyan    = 36,
    White   = 37,
    Default = 0
};

struct style {
    TextStyle value;
};

struct os_format {
    bool bold = false;
    bool underline = false;
    Colour foregroundColour = Colour::White;
};

inline string getSequence(const TextStyle & ts, const Colour & c) {
    string sequence = "";
    bool isFirstSequence = true;

    if (static_cast<int>(ts) & static_cast<int>(TextStyle::Bold)) {
        sequence += "1";
        isFirstSequence = false;
    }
    if (static_cast<int>(ts) & static_cast<int>(TextStyle::Underline)) {
        if (!isFirstSequence) {
            sequence += ";";
        }

        sequence += "4";
        isFirstSequence = false;
    }
    if (static_cast<int>(ts) & static_cast<int>(TextStyle::Inverse)) {
        if (!isFirstSequence) {
            sequence += ";";
        }

        sequence += "7";
        isFirstSequence = false;
    }

    if (c == Colour::Default) {
        return sequence;
    }

    if (!isFirstSequence) {
        sequence += ";";
    }

    return sequence + to_string(static_cast<int>(c));
}

/*
** Shamelessly copied the implementation style of setw() from the
** standard library header <iomanip>
*/
class CustomModifier {
    TextStyle ts;
    Colour c = Colour::Default;

    public:
        explicit CustomModifier(const TextStyle & style) {
            ts = style;
            c = Colour::Default;
        }
        explicit CustomModifier(const TextStyle & style, const Colour & colour) {
            ts = style;
            c = colour;
        }

        template <class _CharT, class _Traits>
        friend basic_ostream<_CharT, _Traits>&
        operator<<(basic_ostream<_CharT, _Traits> & os, const CustomModifier & cm) {
            return os << ESC_CHAR << SEQ_OPEN << getSequence(cm.ts, cm.c) << "m";
        }        
};

inline CustomModifier set_style(const TextStyle & style) {
    return CustomModifier(style);
}

inline CustomModifier set_style(const TextStyle & style, const Colour & colour) {
    return CustomModifier(style, colour);
}

inline std::ostream & operator<<(std::ostream & os, const os_format & f) {
    os << ESC_CHAR << SEQ_OPEN;
    bool first = true;

    if (f.bold) {
        os << "1";
        first = false;
    }
    if (f.underline) {
        if (!first) {
            os << ";";
        }

        os << "4";
        first = false;
    }
    if (!first) {
        os << ";";
    }

    os << static_cast<int>(f.foregroundColour) << "m";

    return os;
}
#else
inline std::ostream & operator<<(std::ostream & os, const os_format & f) {
    return os << "";
}
#endif
