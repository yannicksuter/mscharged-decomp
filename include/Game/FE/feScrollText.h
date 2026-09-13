#ifndef GAME_FE_FE_SCROLL_TEXT_H
#define GAME_FE_FE_SCROLL_TEXT_H

#include "NL/nlBasicString.h"
#include "NL/nlFunction.h"

class TLTextInstance;
class nlFont;

class FEScrollText
{
public:
    FEScrollText(int axis);

    void ApplyNewTextInstancePointer(TLTextInstance* controltext, int pos, int width, int);
    void RefreshText();
    void SetDisplayMessage(const BasicString<unsigned short, Detail::TempStringAllocator>& theMessage);
    void SetDisplayMessage(const char* locMessage);
    void Update(float fDeltaT);
    void SetScrollMode(int mode);
    void SetScrollDirection(int direction);
    void SetEndBehavior(int behavior);
    void SetMetrics(int pos);
    void SetClippingTextInstance(TLTextInstance* controlText);
    int GetScrollSteps(float fDeltaT) const;
    void SetScrollSpeed(float speed) { m_scrollSpeed = speed; }

    /* 0x00 */ TLTextInstance* m_controlText;
    /* 0x04 */ BasicString<unsigned short, Detail::TempStringAllocator> m_message;
    /* 0x08 */ int m_messageWidth;
    /* 0x0C */ int m_pos;
    /* 0x10 */ int m_width;
    /* 0x14 */ int m_scrollDirection;
    /* 0x18 */ int m_scrollAxis;
    /* 0x1C */ int m_scrollMode;
    /* 0x20 */ int m_endBehavior;
    /* 0x24 */ float m_scrollSpeed;
    /* 0x28 */ float m_scrollOffset;
    /* 0x2C */ float m_nextDeltaT;
    /* 0x30 */ Function<FnVoidVoid> m_messageFinishedCB;
    /* 0x38 */ bool m_useMessage;
    /* 0x3C */ nlFont* m_textFont;
}; // size 0x40

#endif // GAME_FE_FE_SCROLL_TEXT_H
