#pragma once

#include <iostream>

const char ESC_CHAR = '\x1B';
const char SEQ_OPEN = '[';
const char SEQ_CLOSE = 'm';

/*
cout << set_style(TextStyle::Bold | TextStyle::Underline, Colour::Red) << "Text" << set_style(TextStyle::Reset) << endl;
*/
enum TextStyle {
    Reset      = 0x00,      // 0
    Bold       = 0x01,      // 1
    Underline  = 0x02,      // 4
    Inverse    = 0x04,      // 7
    NoStyle    = 0x80       // -
};

enum Colour {
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

inline std::string getSequence(const uint8_t ts, const Colour & c) {
    string sequence = "";
    bool isFirstSequence = true;

    if (ts & static_cast<uint8_t>(TextStyle::Bold)) {
        sequence += "1";
        isFirstSequence = false;
    }
    if (ts & static_cast<uint8_t>(TextStyle::Underline)) {
        if (!isFirstSequence) {
            sequence += ";";
        }

        sequence += "4";
        isFirstSequence = false;
    }
    if (ts & static_cast<uint8_t>(TextStyle::Inverse)) {
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
    uint8_t ts = TextStyle::NoStyle;
    Colour c = Colour::Default;

    public:
        explicit CustomModifier(const uint8_t style) {
            ts = style;
            c = Colour::Default;
        }
        explicit CustomModifier(const Colour & colour) {
            c = colour;
        }
        explicit CustomModifier(const uint8_t style, const Colour & colour) {
            ts = style;
            c = colour;
        }

        template <class _CharT, class _Traits>
        friend basic_ostream<_CharT, _Traits>&
        operator<<(basic_ostream<_CharT, _Traits> & os, const CustomModifier & cm) {
#if defined(__APPLE__) || defined(__unix__)
            return os << ESC_CHAR << SEQ_OPEN << getSequence(cm.ts, cm.c) << "m";
#else
            return os << "";
#endif
        }        
};

inline CustomModifier set_style(const uint8_t style) {
    return CustomModifier(style);
}

inline CustomModifier set_style(const Colour & colour) {
    return CustomModifier(colour);
}

inline CustomModifier set_style(const uint8_t style, const Colour & colour) {
    return CustomModifier(style, colour);
}
