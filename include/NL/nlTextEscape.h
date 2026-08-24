#ifndef NL_TEXT_ESCAPE_H
#define NL_TEXT_ESCAPE_H

#include "NL/nlColour.h"

enum ESCAPE_TYPE
{
    ESC_UNKNOWN = 0,
    ESC_OPENBRACE = 1,
    ESC_NON_BREAKING_SPACE = 2,
    ESC_COLOUR = 3,
    ESC_PARAGRAPH = 4,
    ESC_COUNT = 5,
};

class nlEscapeSequence
{
public:
    nlEscapeSequence(const unsigned short* str);

    /* 0x00 */ ESCAPE_TYPE m_Type;
    /* 0x04 */ unsigned short m_Extended[16];
    /* 0x24 */ const unsigned short* m_pEnd;

    static const unsigned short ESCAPE_BEGIN;
};

#endif // NL_TEXT_ESCAPE_H
