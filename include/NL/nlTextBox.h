#ifndef NL_TEXT_BOX_H
#define NL_TEXT_BOX_H

#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/nlColour.h"
#include "NL/nlFont.h"

struct Row
{
    /* 0x0 */ signed short XOffset;
    /* 0x2 */ unsigned short FirstChar;
};

class nlTextBox
{
public:
    enum DrawOptions
    {
        AlignLeft = 0,
        AlignCenter = 1,
        AlignRight = 2,
        VAlignTop = 0,
        VAlignCenter = 0x10,
        VAlignBottom = 0x20,
        ClipBottom = 0x100,
        UseFullHeight = 0x200,
        DontWrapOnSpaces = 0x400,
        FlipY = 0x800,
    };

    struct StringDrawInfo
    {
        static const unsigned long MAX_ROWS = 16;

        /* 0x00 */ const nlFont* pFont;
        /* 0x04 */ const unsigned short* String;
        /* 0x08 */ const nlMatrix4* pMatrix;
        /* 0x0C */ unsigned long DrawOptions;
        /* 0x10 */ unsigned short RowCount;
        /* 0x12 */ signed short YOffset;
        /* 0x14 */ Row Rows[MAX_ROWS + 1];
    };

    static void ProcessString(const FontCharString* pString, const nlFont* pFont, const nlVector2& BoxSize, unsigned long DrawOptions, const nlMatrix4* pMatrix, nlTextBox::StringDrawInfo& DrawInfo);
    static void DrawString(const nlTextBox::StringDrawInfo& DrawInfo, const nlVector2& DrawAt, const nlColour& Color, eGLView View);
};

#endif // NL_TEXT_BOX_H
