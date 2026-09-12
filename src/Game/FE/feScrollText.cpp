#include "Game/FE/feScrollText.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Font/fontmanager.h"
#include "NL/nlFont.inl"
#include "Game/MathHelpers.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/gl/glStruct.h"
#include "NL/nlLexicalCast.h"

static float TEXT_TIME = 4.5f;
static float sScrollTextBoxWidth = 8000.0f;

FEScrollText::FEScrollText(int axis)
    : m_controlText(0)
    , m_message(LexicalCast<const unsigned short*>(L""))
    , m_messageWidth(0)
    , m_pos(-1)
    , m_width(-1)
    , m_scrollDirection(-1)
    , m_scrollAxis(axis)
    , m_scrollMode(0)
    , m_endBehavior(0)
    , m_scrollSpeed(0.0f)
    , m_scrollOffset(0.0f)
    , m_nextDeltaT(0.0f)
    , m_textFont(0)
{
    m_useMessage = false;
}

void FEScrollText::SetDisplayMessage(const BasicString<unsigned short, Detail::TempStringAllocator>& theMessage)
{
    m_useMessage = true;
    m_message = theMessage;
    m_controlText->SetString(m_message.c_str());
    RefreshText();
}

void FEScrollText::SetDisplayMessage(const char* locMessage)
{
    const unsigned short* text = LookupLocString(locMessage);
    SetDisplayMessage(BasicString<unsigned short, Detail::TempStringAllocator>(text));
}

void FEScrollText::SetScrollMode(int mode)
{
    m_scrollMode = mode;
}

void FEScrollText::SetScrollDirection(int direction)
{
    m_scrollDirection = direction;
}

void FEScrollText::SetEndBehavior(int behavior)
{
    m_endBehavior = behavior;
}

void FEScrollText::SetClippingTextInstance(TLTextInstance* controlText)
{
    if (m_scrollAxis == 0)
    {
        nlVector2& boxSize = (controlText->m_OverloadFlags & 0x4)
            ? controlText->m_OverloadedAttributes.BoxSize
            : ((FEText*)controlText->m_component)->m_TextAttributes.BoxSize;
        m_width = (int)boxSize.x;
        SetMetrics((int)(controlText->GetPosition().f.x - controlText->GetPivot().f.x));
    }
    else
    {
        nlVector2& boxSize = (controlText->m_OverloadFlags & 0x4)
            ? controlText->m_OverloadedAttributes.BoxSize
            : ((FEText*)controlText->m_component)->m_TextAttributes.BoxSize;
        m_width = (int)boxSize.y;
        SetMetrics((int)(controlText->GetPosition().f.y - controlText->GetPivot().f.y));
    }
}

void FEScrollText::ApplyNewTextInstancePointer(TLTextInstance* controltext, int pos, int width, int)
{
    if (m_controlText != 0)
    {
        feVector3 oldPosition = m_controlText->GetPosition();
        if (m_scrollAxis == 0)
        {
            m_controlText->SetAssetPosition((float)m_pos, oldPosition.f.y, oldPosition.f.z);
        }
        else
        {
            m_controlText->SetAssetPosition(oldPosition.f.x, (float)m_pos, oldPosition.f.z);
        }
    }

    m_controlText = controltext;
    if (pos == -1 && width == -1)
    {
        if (m_pos == -1 && m_width == -1)
        {
            if (m_scrollAxis == 0)
            {
                m_pos = (int)controltext->GetAssetPosition().f.x;
            }
            else
            {
                m_pos = (int)controltext->GetPosition().f.y;
            }
            SetClippingTextInstance(controltext);
        }
    }
    else
    {
        m_width = width;
        m_pos = pos - width / 2;
        SetMetrics(m_pos);
    }

    m_scrollSpeed = (float)m_width / TEXT_TIME;
    if (IsWidescreen())
    {
        float aspectRatio = 854.0f / 640.0f;
        m_scrollSpeed *= aspectRatio;
    }

    if (m_scrollAxis == 0)
    {
        nlVector2 boxSize;
        boxSize.x = sScrollTextBoxWidth;
        boxSize.y = 100.0f;
        TLTextInstance* text = m_controlText;
        text->m_OverloadedAttributes.BoxSize = boxSize;
        text->m_OverloadFlags |= 0x4;
    }
    m_controlText->m_bVisible = false;
    m_textFont = 0;
    RefreshText();
}

void FEScrollText::RefreshText()
{
    if (m_textFont == 0)
    {
        const FEFontResource* resource = ((FEText*)m_controlText->m_component)->m_pFeFontResource;
        if (resource == 0)
        {
            m_textFont = FontManager::Instance()->GetFontByHashID(0);
        }
        else
        {
            m_textFont = resource->m_pFontReference;
        }
        if (m_textFont == 0)
        {
            return;
        }
    }

    const unsigned short* text;
    if (m_useMessage)
    {
        text = m_message.c_str();
    }
    else
    {
        text = m_controlText->GetString();
    }
    if (text == 0)
    {
        text = (const unsigned short*)L"";
    }

    if (m_scrollAxis == 0)
    {
        m_messageWidth = m_textFont->GetStringWidth(BasicString<unsigned short, Detail::TempStringAllocator>(text), true, 640, true);
        m_messageWidth = (int)((float)m_messageWidth * m_controlText->GetScale().f.x);
    }
    else
    {
        m_messageWidth = m_textFont->fn_80305278(BasicString<unsigned short, Detail::TempStringAllocator>(text),
            (unsigned long)((m_controlText->m_OverloadFlags & 0x4)
                    ? m_controlText->m_OverloadedAttributes.BoxSize
                    : ((FEText*)m_controlText->m_component)->m_TextAttributes.BoxSize).x,
            true);
        m_messageWidth = (int)((float)m_messageWidth * m_controlText->GetScale().f.y);
    }

    if (m_scrollMode == 0)
    {
        if (m_scrollDirection == -1)
        {
            m_scrollOffset = (float)m_width + m_scrollSpeed;
        }
        else
        {
            m_scrollOffset = (float)-m_messageWidth - m_scrollSpeed;
        }
    }
    else
    {
        if (m_scrollDirection == 1)
        {
            m_scrollOffset = (float)nlMin(m_width - m_messageWidth, 0);
        }
        if (m_scrollDirection == -1)
        {
            m_scrollOffset = 0.0f;
        }
    }
    Update(0.0f);
}

void FEScrollText::Update(float fDeltaT)
{
    if (m_textFont == 0)
    {
        RefreshText();
        if (m_textFont == 0)
        {
            return;
        }
    }

    m_controlText->m_bVisible = true;
    if (m_nextDeltaT > 0.0f)
    {
        fDeltaT = m_nextDeltaT;
        m_nextDeltaT = 0.0f;
    }
    m_scrollOffset += m_scrollDirection * (fDeltaT * m_scrollSpeed);

    float minOffset;
    float maxOffset;
    if (m_scrollMode == 0)
    {
        minOffset = (float)-m_messageWidth;
        maxOffset = (float)m_width;
        if (m_scrollDirection == -1 && m_scrollOffset < minOffset)
        {
            m_scrollOffset = minOffset;
        }
        else if (m_scrollDirection == 1 && m_scrollOffset > maxOffset)
        {
            m_scrollOffset = maxOffset;
        }
    }
    else
    {
        if (m_messageWidth > m_width)
        {
            minOffset = 0.0f;
            maxOffset = (float)(m_messageWidth - m_width);
        }
        else
        {
            minOffset = (float)(m_width - m_messageWidth);
            maxOffset = 0.0f;
        }
        m_scrollOffset = nlMinEquals(nlMaxEquals(m_scrollOffset, minOffset), maxOffset);
    }

    bool finished = false;
    if (m_scrollDirection == -1)
    {
        finished = m_scrollOffset == minOffset;
    }
    else if (m_scrollDirection == 1)
    {
        finished = m_scrollOffset == maxOffset;
    }

    feVector3 position = m_controlText->GetPosition();
    if (m_scrollAxis == 0)
    {
        m_controlText->SetAssetPosition((float)m_pos + m_scrollOffset, position.f.y, position.f.z);
    }
    else
    {
        m_controlText->SetAssetPosition(position.f.x, (float)m_pos + m_scrollOffset, position.f.z);
    }

    if (finished)
    {
        switch (m_endBehavior)
        {
        case 0:
            if (m_scrollMode == 0)
            {
                if (m_scrollDirection == -1)
                {
                    m_scrollOffset = (float)m_width + m_scrollSpeed;
                }
                else
                {
                    m_scrollOffset = (float)-m_messageWidth - m_scrollSpeed;
                }
            }
            else
            {
                if (m_scrollDirection == 1)
                {
                    m_scrollOffset = (float)nlMin(m_width - m_messageWidth, 0);
                }
                if (m_scrollDirection == -1)
                {
                    m_scrollOffset = 0.0f;
                }
            }
            Update(0.0f);
            break;
        case 1:
            m_scrollDirection = m_scrollDirection == -1 ? 1 : -1;
            if (m_scrollMode == 0)
            {
                if (m_scrollDirection == -1)
                {
                    m_scrollOffset = (float)m_width + m_scrollSpeed;
                }
                else
                {
                    m_scrollOffset = (float)-m_messageWidth - m_scrollSpeed;
                }
            }
            else
            {
                if (m_scrollDirection == 1)
                {
                    m_scrollOffset = (float)nlMin(m_width - m_messageWidth, 0);
                }
                if (m_scrollDirection == -1)
                {
                    m_scrollOffset = 0.0f;
                }
            }
            Update(0.0f);
            break;
        case 2:
            m_scrollDirection = 0;
            break;
        }
        if (m_messageFinishedCB)
        {
            m_messageFinishedCB();
        }
    }
}

void FEScrollText::SetMetrics(int pos)
{
    const gl_ScreenInfo* screenInfo = glGetScreenInfo();
    int screenWidth = IsWidescreen() ? 854.0f : screenInfo->ScreenWidth;
    int screenHeight = screenInfo->ScreenHeight;
    int boxPos;
    if (pos == -9999)
    {
        if (m_scrollAxis == 0)
            boxPos = m_controlText->m_ScissorRect.X;
        else
            boxPos = m_controlText->m_ScissorRect.Y;
    }
    else if (m_scrollAxis == 0)
    {
        boxPos = screenWidth / 2 + pos;
        if (IsWidescreen())
            boxPos = (int)(boxPos * (640.0f / 854.0f));
    }
    else
    {
        boxPos = screenHeight / 2 - pos;
    }

    int boxWidth = m_width;
    if (m_scrollAxis == 0 && IsWidescreen())
        boxWidth = (int)(boxWidth * (640.0f / 854.0f));

    if (boxPos < 0)
        boxPos = 0;
    if (m_scrollAxis == 0 && boxWidth + boxPos >= screenInfo->ScreenWidth)
        boxWidth = screenInfo->ScreenWidth - boxPos - 1;
    else if (m_scrollAxis == 1 && boxWidth + boxPos >= screenInfo->ScreenHeight)
        boxWidth = screenInfo->ScreenHeight - boxPos - 1;

    if (m_scrollAxis == 0)
        m_controlText->SetScissorBox((u16)boxPos, 0, (u16)boxWidth, (u16)screenInfo->ScreenHeight);
    else
        m_controlText->SetScissorBox(0, (u16)boxPos, (u16)screenInfo->ScreenWidth, (u16)boxWidth);
}

int FEScrollText::GetScrollSteps(float fDeltaT) const
{
    float distance = m_messageWidth - m_width;
    return (int)(distance / (m_scrollSpeed * fDeltaT));
}
