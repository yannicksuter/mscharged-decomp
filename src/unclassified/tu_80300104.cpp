#include "unclassified/tu_80300104.h"

#include "Game/FE/feInput.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/MathHelpers.h"
#include "NL/gl/glStruct.h"
#include "NL/nlBasicString.h"
#include "NL/nlFont.h"

struct TU802FA1C4;

extern TU802FA1C4* lbl_806E2030;
extern void* lbl_806E2090;

extern "C" void fn_802FA2FC(TU802FA1C4* owner, TU80300104Base* listener);
extern "C" void fn_802FA388(TU802FA1C4* owner, TU80300104Base* listener);
extern "C" nlFont* fn_80307260(void* owner, int index);
extern "C" unsigned int fn_80304CE4(nlFont* font, const FontCharString* text, int start, int width, bool includeTrailingSpace);

nlVector2 fn_80301518(TLTextInstance* text);
nlVector2 fn_803018F0(TLInstance* first);

TU80300104Base::TU80300104Base(void* context)
    : mContext(context)
    , mDisabled(false)
    , mIgnoreInputLock(false)
{
    fn_802FA2FC(lbl_806E2030, this);
}

TU80300104Base::~TU80300104Base()
{
    fn_802FA388(lbl_806E2030, this);
}

void TU80300104Base::fn_80300594(const TU80300104Event* event)
{
    if (mDisabled)
    {
        return;
    }

    if (g_pFEInput->m_InputLockDepth == 0 || mIgnoreInputLock)
    {
        if (fn_8030131C(event->mPosition))
        {
            if (!fn_8030131C(mPreviousEvents[event->mIndex].mPosition))
            {
                fn_80301BA8(event->mIndex, mContext);
            }

            fn_80301BE8(event->mIndex, mContext);
            fn_80301C28(event->mIndex, mContext);

            if (event->mFlag0)
            {
                fn_80301C68(event->mIndex, mContext);
            }

            if (event->mFlag2)
            {
                fn_80301CA8(event->mIndex, mContext);
            }
        }
        else if (fn_8030131C(mPreviousEvents[event->mIndex].mPosition))
        {
            fn_80301CE8(event->mIndex, mContext);
        }

        if (event->mFlag1)
        {
            fn_80301D28(event->mIndex, mContext);
        }

        mPreviousEvents[event->mIndex] = *event;
    }
}

void TU80300104Base::fn_803007C0(const Callback& callback)
{
    mCallback1 = callback;
}

void TU80300104Base::fn_80300864(const Callback& callback)
{
    mCallback2 = callback;
}

void TU80300104Base::fn_80300908(const Callback& callback)
{
    mCallback3 = callback;
}

void TU80300104Base::fn_803009AC(const Callback& callback)
{
    mCallback4 = callback;
}

void TU80300104Base::fn_80300A50(const Callback& callback)
{
    mCallback5 = callback;
}

TU80300104Component::TU80300104Component(void* context)
    : TU80300104Base(context)
    , mMinX(0.0f)
    , mMaxX(0.0f)
    , mMaxY(0.0f)
    , mMinY(0.0f)
    , mRotation(0.0f)
{
}

TU80300104Component::~TU80300104Component()
{
}

void TU80300104Component::fn_80300D74(TLInstance* instance, bool useRotation, float offsetX, float offsetY, float scaleX, float scaleY)
{
    nlVector2 size;
    nlVector2 measuredSize;
    switch (instance->m_type)
    {
    case TLAT_LAYER:
        measuredSize = fn_803018F0(instance->pChildren);
        break;
    case TLAT_IMAGE:
        measuredSize.y = instance->GetScale().f.y * 100.0f;
        measuredSize.x = instance->GetScale().f.x * 100.0f;
        break;
    case TLAT_TEXT:
    {
        TLTextInstance* text = (TLTextInstance*)instance;
        const FEFontResource* fontResource = ((const FEText*)text->m_component)->m_pFeFontResource;
        nlFont* font = fontResource == 0 ? fn_80307260(lbl_806E2090, 0) : fontResource->m_pFontReference;

        float width;
        {
            BasicString<unsigned short, Detail::TempStringAllocator> string(text->GetString());
            FontCharString fontString(string.c_str(), font, (unsigned short*)0);
            width = fn_80304CE4(font, &fontString, 0, 640, true);
        }
        nlTextBox::StringDrawInfo drawInfo = text->m_DrawInfo;
        nlVector2 textSize;
        textSize.x = width;
        textSize.y = (float)(font->m_Metrics.Height * drawInfo.RowCount);
        measuredSize = textSize;
        break;
    }
    case TLAT_COMPONENT:
        measuredSize = fn_803018F0(((TLComponentInstance*)instance)->GetActiveSlide()->m_instances);
        break;
    case TLAT_GROUP:
        measuredSize = fn_803018F0(instance->pChildren);
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

bool TU80300104Component::fn_8030131C(nlVector2 position) const
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

nlVector2 fn_80301518(TLTextInstance* text)
{
    const FEFontResource* fontResource = ((const FEText*)text->m_component)->m_pFeFontResource;
    nlFont* font;
    if (fontResource == 0)
    {
        font = fn_80307260(lbl_806E2090, 0);
    }
    else
    {
        font = fontResource->m_pFontReference;
    }

    float width;
    {
        BasicString<unsigned short, Detail::TempStringAllocator> string(text->GetString());
        FontCharString fontString(string.c_str(), font, (unsigned short*)0);
        width = fn_80304CE4(font, &fontString, 0, 640, true);
    }
    nlTextBox::StringDrawInfo drawInfo = text->m_DrawInfo;
    nlVector2 size;
    size.x = width;
    size.y = (float)(font->m_Metrics.Height * drawInfo.RowCount);
    return size;
}

nlVector2 fn_803018F0(TLInstance* first)
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
            measuredSize = fn_803018F0(instance->pChildren);
            break;
        case TLAT_IMAGE:
            measuredSize.y = instance->GetScale().f.y * 100.0f;
            measuredSize.x = instance->GetScale().f.x * 100.0f;
            break;
        case TLAT_TEXT:
            measuredSize = fn_80301518((TLTextInstance*)instance);
            break;
        case TLAT_COMPONENT:
            measuredSize = fn_803018F0(((TLComponentInstance*)instance)->GetActiveSlide()->m_instances);
            break;
        case TLAT_GROUP:
            measuredSize = fn_803018F0(instance->pChildren);
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

void TU80300104Base::fn_80301BA8(int index, void* context)
{
    if (mCallback1)
    {
        mCallback1(index, context);
    }
}

void TU80300104Base::fn_80301BE8(int index, void* context)
{
    if (mCallback0)
    {
        mCallback0(index, context);
    }
}

void TU80300104Base::fn_80301C28(int index, void* context)
{
    if (mCallback3)
    {
        mCallback3(index, context);
    }
}

void TU80300104Base::fn_80301C68(int index, void* context)
{
    if (mCallback4)
    {
        mCallback4(index, context);
    }
}

void TU80300104Base::fn_80301CA8(int index, void* context)
{
    if (mCallback6)
    {
        mCallback6(index, context);
    }
}

void TU80300104Base::fn_80301CE8(int index, void* context)
{
    if (mCallback2)
    {
        mCallback2(index, context);
    }
}

void TU80300104Base::fn_80301D28(int index, void* context)
{
    if (mCallback5)
    {
        mCallback5(index, context);
    }
}
