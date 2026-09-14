#include "Game/FE/fePointer.h"
#include "NL/nlBasicString.inl"

#include "Game/FE/feInput.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/fePointerManager.h"
#include "Game/Font/fontmanager.h"
#include "Game/MathHelpers.h"
#include "NL/gl/glStruct.h"
#include "NL/nlBasicString.h"
#include "NL/nlFont.h"
#include "NL/nlstring_tmpl.h"

nlVector2 MeasurePointerText(TLTextInstance* text);
nlVector2 MeasurePointerInstanceList(TLInstance* first);

FEPointerListener::FEPointerListener(void* context)
    : mContext(context)
    , mDisabled(false)
    , mIgnoreInputLock(false)
{
    g_pFEPointerManager->RegisterListener(this);
}

FEPointerListener::~FEPointerListener()
{
    g_pFEPointerManager->UnregisterListener(this);
}

void FEPointerListener::ProcessPointerEvent(const FEPointerEvent* event)
{
    if (mDisabled || (g_pFEInput->m_InputLockDepth != 0 && !mIgnoreInputLock))
    {
        return;
    }

    if (ContainsPoint(event->mPosition))
    {
        if (!ContainsPoint(mPreviousEvents[event->mIndex].mPosition))
        {
            OnPointerEnter(event->mIndex, mContext);
        }

        OnPointerUpdate(event->mIndex, mContext);
        OnPointerInside(event->mIndex, mContext);

        if (event->mPressed)
        {
            OnPointerPress(event->mIndex, mContext);
        }

        if (event->mSecondaryAction)
        {
            OnPointerSecondaryAction(event->mIndex, mContext);
        }
    }
    else if (ContainsPoint(mPreviousEvents[event->mIndex].mPosition))
    {
        OnPointerLeave(event->mIndex, mContext);
    }

    if (event->mReleased)
    {
        OnPointerRelease(event->mIndex, mContext);
    }

    mPreviousEvents[event->mIndex] = *event;
}

void FEPointerListener::SetPointerEnterCallback(const Callback& callback)
{
    mEnterCallback = callback;
}

void FEPointerListener::SetPointerLeaveCallback(const Callback& callback)
{
    mLeaveCallback = callback;
}

void FEPointerListener::SetPointerInsideCallback(const Callback& callback)
{
    mInsideCallback = callback;
}

void FEPointerListener::SetPointerPressCallback(const Callback& callback)
{
    mPressCallback = callback;
}

void FEPointerListener::SetPointerReleaseCallback(const Callback& callback)
{
    mReleaseCallback = callback;
}

FEPointerRegion::FEPointerRegion(void* context)
    : FEPointerListener(context)
    , mMinX(0.0f)
    , mMaxX(0.0f)
    , mMaxY(0.0f)
    , mMinY(0.0f)
    , mRotation(0.0f)
{
}

FEPointerRegion::~FEPointerRegion()
{
}

void FEPointerRegion::SetInstanceBounds(TLInstance* instance, bool useRotation, float offsetX, float offsetY, float scaleX, float scaleY)
{
    nlVector2 size;
    nlVector2 measuredSize;
    switch (instance->m_type)
    {
    case TLAT_LAYER:
        measuredSize = MeasurePointerInstanceList(instance->pChildren);
        break;
    case TLAT_IMAGE:
        measuredSize.y = instance->GetScale().f.y * 100.0f;
        measuredSize.x = instance->GetScale().f.x * 100.0f;
        break;
    case TLAT_TEXT:
    {
        TLTextInstance* text = (TLTextInstance*)instance;
        const FEFontResource* fontResource = ((const FEText*)text->m_component)->m_pFeFontResource;
        nlFont* font = fontResource == 0 ? FontManager::Instance()->GetFontByHashID(0) : fontResource->m_pFontReference;

        float width;
        {
            BasicString<unsigned short, Detail::TempStringAllocator> string(text->GetString());
            FontCharString fontString(string.c_str(), font, (unsigned short*)0);
            width = font->GetStringWidth(fontString, false, 640, true);
        }
        nlTextBox::StringDrawInfo drawInfo = text->m_DrawInfo;
        nlVector2 textSize;
        textSize.x = width;
        textSize.y = (float)(font->m_Metrics.Height * drawInfo.RowCount);
        measuredSize = textSize;
        break;
    }
    case TLAT_COMPONENT:
        measuredSize = MeasurePointerInstanceList(((TLComponentInstance*)instance)->GetActiveSlide()->pChildren);
        break;
    case TLAT_GROUP:
        measuredSize = MeasurePointerInstanceList(instance->pChildren);
        break;
    default:
        nlVec2Set(measuredSize, 0.0f, 0.0f);
        break;
    }

    size = measuredSize;
    size.x *= scaleX;
    size.y *= scaleY;

    feVector3 position = instance->GetAssetPosition();
    float x = position.f.x + offsetX;
    float y = position.f.y + offsetY;
    mMinX = x - size.x * 0.5f;
    mMaxX = x + size.x * 0.5f;
    mMaxY = y + size.y * 0.5f;
    mMinY = y - size.y * 0.5f;

    if (useRotation)
    {
        mRotation = instance->GetAssetRotation().f.z;
        mPivotX = position.f.x;
        mPivotY = position.f.y;
    }
    else
    {
        mRotation = 0.0f;
    }
}

bool FEPointerRegion::ContainsPoint(nlVector2 position) const
{
    if (mRotation == 0.0f)
    {
        return position.x >= mMinX && position.x <= mMaxX && position.y >= mMinY && position.y <= mMaxY;
    }

    nlVector3 local;
    nlVec3Set(local, position.x - mPivotX, position.y - mPivotY, 0.0f);

    float cosine = nlSin((unsigned short)(RadToAng16(mRotation) + 0x4000));
    float sineForY = nlSin(RadToAng16(mRotation));
    float rotatedY = local.x * -sineForY + local.y * cosine;
    float sine = nlSin(RadToAng16(mRotation));
    cosine = nlSin((unsigned short)(RadToAng16(mRotation) + 0x4000));
    float rotatedX = local.x * cosine + local.y * sine;
    nlVec3Set(local, rotatedX, rotatedY, 0.0f);
    nlVec3Set(local, local.x + mPivotX, local.y + mPivotY, 0.0f);

    return local.x >= mMinX && local.x <= mMaxX && local.y >= mMinY && local.y <= mMaxY;
}

nlVector2 MeasurePointerText(TLTextInstance* text)
{
    const FEFontResource* fontResource = ((const FEText*)text->m_component)->m_pFeFontResource;
    nlFont* font;
    if (fontResource == 0)
    {
        font = FontManager::Instance()->GetFontByHashID(0);
    }
    else
    {
        font = fontResource->m_pFontReference;
    }

    float width;
    {
        BasicString<unsigned short, Detail::TempStringAllocator> string(text->GetString());
        FontCharString fontString(string.c_str(), font, (unsigned short*)0);
        width = font->GetStringWidth(fontString, false, 640, true);
    }
    nlTextBox::StringDrawInfo drawInfo = text->m_DrawInfo;
    nlVector2 size;
    size.x = width;
    size.y = (float)(font->m_Metrics.Height * drawInfo.RowCount);
    return size;
}

nlVector2 MeasurePointerInstanceList(TLInstance* first)
{
    if (first == 0)
    {
        nlVector2 size;
        nlVec2Set(size, 0.0f, 0.0f);
        return size;
    }

    gl_ScreenInfo* screen = glGetScreenInfo();
    float minX = 427.0f;
    float minY = (float)(screen->ScreenHeight / 2);
    float maxX = -minX;
    float maxY = -minY;

    TLInstance* instance = first;
    do
    {
        nlVector2 measuredSize;
        switch (instance->m_type)
        {
        case TLAT_LAYER:
            measuredSize = MeasurePointerInstanceList(instance->pChildren);
            break;
        case TLAT_IMAGE:
            measuredSize.y = instance->GetScale().f.y * 100.0f;
            measuredSize.x = instance->GetScale().f.x * 100.0f;
            break;
        case TLAT_TEXT:
            measuredSize = MeasurePointerText((TLTextInstance*)instance);
            break;
        case TLAT_COMPONENT:
            measuredSize = MeasurePointerInstanceList(((TLComponentInstance*)instance)->GetActiveSlide()->pChildren);
            break;
        case TLAT_GROUP:
            measuredSize = MeasurePointerInstanceList(instance->pChildren);
            break;
        default:
            nlVec2Set(measuredSize, 0.0f, 0.0f);
            break;
        }

        nlVector2 size = measuredSize;
        feVector3 position = instance->GetAssetPosition();
        float left = position.f.x - size.x * 0.5f;
        float right = position.f.x + size.x * 0.5f;
        float bottom = position.f.y - size.y * 0.5f;
        float top = position.f.y + size.y * 0.5f;

        minX = left < minX ? left : minX;
        minY = bottom < minY ? bottom : minY;
        maxX = right > maxX ? right : maxX;
        maxY = top > maxY ? top : maxY;

        instance = instance->m_next;
    } while (instance != first);

    nlVector2 size;
    nlVec2Set(size, maxX - minX, maxY - minY);
    return size;
}

void FEPointerListener::OnPointerEnter(int index, void* context)
{
    if (mEnterCallback)
    {
        mEnterCallback(index, context);
    }
}

void FEPointerListener::OnPointerUpdate(int index, void* context)
{
    if (mUpdateCallback)
    {
        mUpdateCallback(index, context);
    }
}

void FEPointerListener::OnPointerInside(int index, void* context)
{
    if (mInsideCallback)
    {
        mInsideCallback(index, context);
    }
}

void FEPointerListener::OnPointerPress(int index, void* context)
{
    if (mPressCallback)
    {
        mPressCallback(index, context);
    }
}

void FEPointerListener::OnPointerSecondaryAction(int index, void* context)
{
    if (mSecondaryActionCallback)
    {
        mSecondaryActionCallback(index, context);
    }
}

void FEPointerListener::OnPointerLeave(int index, void* context)
{
    if (mLeaveCallback)
    {
        mLeaveCallback(index, context);
    }
}

void FEPointerListener::OnPointerRelease(int index, void* context)
{
    if (mReleaseCallback)
    {
        mReleaseCallback(index, context);
    }
}
