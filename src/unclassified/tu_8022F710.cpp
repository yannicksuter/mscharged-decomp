#include "unclassified/tu_8022F710.h"

#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern TLComponentInstance lbl_80580030;
extern TLInstance lbl_80580248;

UnidentifiedScrollWidget::UnidentifiedScrollWidget()
    : mUnidentified0C(0)
    , mUnidentified18(false)
    , mUnidentified19(false)
    , mUnidentified1A(true)
    , mUnidentified1B(false)
    , mUnidentified28(0.0f)
    , mUnidentified2C(0.0f)
    , mUnidentified30(0.0f)
    , mUnidentified34(0)
    , mUnidentified38(0)
    , mUnidentified48(9999.9f)
{
    mComponents[0].mContext = (void*)0;
    mComponents[0].mSpeakerEnabled = false;
    mComponents[1].mContext = (void*)1;
    mComponents[1].mSpeakerEnabled = false;
    mUnidentified10[0] = false;
    mUnidentified10[1] = false;
    mUnidentified14[1] = false;
    mUnidentified14[0] = false;
    mUnidentified16[1] = false;
    mUnidentified16[0] = false;
    mUnidentified3C.x = 0.0f;
    mUnidentified3C.y = 0.0f;
    mUnidentified3C.z = 0.0f;
}

UnidentifiedScrollWidget::~UnidentifiedScrollWidget()
{
}

void UnidentifiedScrollWidget::fn_8022F848(bool enabled)
{
    mUnidentified1B = enabled;
    mComponents[0].mIgnoreInputLock = enabled;
    mComponents[1].mIgnoreInputLock = enabled;
}

void UnidentifiedScrollWidget::fn_8022F858()
{
    mUnidentified18 = true;
    mComponents[0].fn_80300D74(mUnidentified04[0], false, mUnidentified1C.f.x, mUnidentified1C.f.y, 1.0f, 1.0f);
    mComponents[1].fn_80300D74(mUnidentified04[1], false, mUnidentified1C.f.x, mUnidentified1C.f.y, 1.0f, 1.0f);

    TU80300104Base::Callback callback(Bind<void>(MemFun(&UnidentifiedScrollWidget::fn_80230EE0), this, Placeholder<0>(), Placeholder<1>()));
    mComponents[0].fn_803007C0(callback);
    mComponents[1].fn_803007C0(callback);
    callback = TU80300104Base::Callback(Bind<void>(MemFun(&UnidentifiedScrollWidget::fn_8022FDDC), this, Placeholder<0>(), Placeholder<1>()));
    mComponents[0].fn_80300864(callback);
    mComponents[1].fn_80300864(callback);

    TU80300104Base::Callback callback2(Bind<void>(MemFun(&UnidentifiedScrollWidget::fn_8022FE94), this, Placeholder<0>(), Placeholder<1>()));
    mComponents[0].fn_803009AC(callback2);
    mComponents[1].fn_803009AC(callback2);
    callback2 = TU80300104Base::Callback(Bind<void>(MemFun(&UnidentifiedScrollWidget::fn_8022FFA8), this, Placeholder<0>(), Placeholder<1>()));
    mComponents[0].fn_80300A50(callback2);
    mComponents[1].fn_80300A50(callback2);
}

bool UnidentifiedScrollWidget::fn_8022FD80(int direction, int value)
{
    if (mUnidentified10[direction] && mUnidentified28 >= 0.5f)
    {
        if ((mUnidentified14[direction] || mUnidentified16[direction]) && value)
            mUnidentified28 = 0.0f;
        return true;
    }
    return false;
}

void UnidentifiedScrollWidget::fn_8022FDDC(int index, void* context)
{
    int direction = (int)context;
    mComponents[direction].mValues[index] = 0;
    if (context == 0)
    {
        if (mUnidentified34 <= 0)
            return;
    }
    else if (context == (void*)1)
    {
        if (mUnidentified34 >= mUnidentified38)
            return;
    }
    if (!mUnidentified16[direction])
    {
        mUnidentified04[direction]->SetActiveSlide("off", true, false);
        mUnidentified19 = false;
        mUnidentified10[direction] = false;
    }
}

void UnidentifiedScrollWidget::fn_8022FE94(int, void* context)
{
    if (mUnidentified16[0] || mUnidentified16[1])
        return;
    if (context == 0)
    {
        if (mUnidentified34 <= 0 || mUnidentified14[1])
            return;
    }
    else if (context == (void*)1)
    {
        if (mUnidentified34 >= mUnidentified38 || mUnidentified14[0])
            return;
    }
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    mUnidentified04[direction]->SetActiveSlide("slide1", true, false);
    mUnidentified04[other]->SetActiveSlide("off", true, false);
    mUnidentified14[direction] = true;
    mUnidentified10[direction] = true;
    mUnidentified28 = 0.5f;
    fn_801CBCA0(0x3021A1EE, 0, 0, 1);
}

void UnidentifiedScrollWidget::fn_8022FFA8(int index, void* context)
{
    if (mUnidentified16[0] || mUnidentified16[1])
        return;
    mUnidentified10[0] = false;
    mUnidentified14[0] = false;
    mUnidentified10[1] = false;
    mUnidentified14[1] = false;
    mUnidentified28 = 0.0f;
    int direction = (int)context;
    if (context == 0)
    {
        if (mUnidentified34 <= 0)
        {
            mUnidentified04[direction]->SetActiveSlide("off", true, false);
            return;
        }
    }
    else if (context == (void*)1)
    {
        if (mUnidentified34 >= mUnidentified38)
        {
            mUnidentified04[direction]->SetActiveSlide("off", true, false);
            return;
        }
    }
    if (mComponents[direction].mValues[index] == 0)
        mUnidentified04[direction]->SetActiveSlide("off", true, false);
    else if (mComponents[direction].mValues[index] == 1)
        mUnidentified04[direction]->SetActiveSlide("over", true, false);
}

void UnidentifiedScrollWidget::fn_802300A4(int, void* context)
{
    if (mUnidentified14[0] || mUnidentified14[1])
        return;
    if (context == 0)
    {
        if (mUnidentified34 <= 0 || mUnidentified16[1])
            return;
    }
    else if (context == (void*)1)
    {
        if (mUnidentified34 >= mUnidentified38 || mUnidentified16[0])
            return;
    }
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    mUnidentified04[direction]->SetActiveSlide("slide1", true, false);
    mUnidentified04[other]->SetActiveSlide("off", true, false);
    mUnidentified16[direction] = true;
    mUnidentified10[direction] = true;
    mUnidentified28 = 0.5f;
    fn_801CBCA0(0x3021A1EE, 0, 0, 1);
}

void UnidentifiedScrollWidget::fn_802301B8(int index, void* context)
{
    if (mUnidentified14[0] || mUnidentified14[1])
        return;
    mUnidentified10[0] = false;
    mUnidentified16[0] = false;
    mUnidentified10[1] = false;
    mUnidentified16[1] = false;
    mUnidentified28 = 0.0f;
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    if ((context == 0 && mUnidentified34 <= 0) || (context == (void*)1 && mUnidentified34 >= mUnidentified38))
        mUnidentified04[direction]->SetActiveSlide("off", true, false);
    else if (mComponents[direction].mValues[index] == 0)
        mUnidentified04[direction]->SetActiveSlide("off", true, false);
    else if (mComponents[direction].mValues[index] == 1)
        mUnidentified04[direction]->SetActiveSlide("over", true, false);

    if ((other == 0 && mUnidentified34 <= 0) || (other == 1 && mUnidentified34 >= mUnidentified38))
        mUnidentified04[other]->SetActiveSlide("off", true, false);
    else if (mComponents[other].mValues[index] == 0)
        mUnidentified04[other]->SetActiveSlide("off", true, false);
    else if (mComponents[other].mValues[index] == 1)
        mUnidentified04[other]->SetActiveSlide("over", true, false);
}

void UnidentifiedScrollWidget::fn_8023038C(const nlVector3& value)
{
    nlVec3Set(mUnidentified3C, value.x, value.y, value.z);
    mUnidentified1C.f.x += mUnidentified3C.x;
    mUnidentified1C.f.y += mUnidentified3C.y;
    mUnidentified1C.f.z += mUnidentified3C.z;
}

void UnidentifiedScrollWidget::fn_802303CC()
{
    if (mUnidentified48 != 9999.9f && mUnidentified0C != 0)
    {
        feVector3 position = mUnidentified0C->GetAssetPosition();
        position.f.y = mUnidentified48;
        mUnidentified0C->SetAssetPosition(position.f.x, position.f.y, position.f.z);
    }
    mUnidentified10[0] = false;
    mUnidentified10[1] = false;
    mUnidentified14[1] = false;
    mUnidentified14[0] = false;
    mUnidentified16[1] = false;
    mUnidentified16[0] = false;
    mUnidentified28 = 0.0f;
}

void UnidentifiedScrollWidget::fn_80230468(TU80300104Event event, float dt)
{
    if (g_pFEInput->m_InputLockDepth != 0 && !mUnidentified1B)
        return;
    if (mUnidentified10[0] || mUnidentified10[1])
        mUnidentified28 += dt;
    mComponents[0].fn_80219608(&event);
    mComponents[1].fn_80219608(&event);
    eFEINPUT_PAD pad = (eFEINPUT_PAD)event.mIndex;
    if (g_pFEInput->JustPressed(pad, 13, true, 0) && !mUnidentified14[0] && !mUnidentified14[1])
        fn_802300A4(event.mIndex, (void*)0);
    else if (g_pFEInput->JustReleased(pad, 13, true, 0))
        fn_802301B8(event.mIndex, (void*)0);
    else if (g_pFEInput->JustPressed(pad, 14, true, 0) && !mUnidentified14[0] && !mUnidentified14[1])
        fn_802300A4(event.mIndex, (void*)1);
    else if (g_pFEInput->JustReleased(pad, 14, true, 0))
        fn_802301B8(event.mIndex, (void*)1);

    if (fn_8022FD80(0, false))
    {
        if (mUnidentified34 > 0)
        {
            --mUnidentified34;
            fn_801CBCA0(0x3021A1EE, 0, 0, 1);
            feVector3 position = mUnidentified0C->GetAssetPosition();
            float offset = mUnidentified34 * mUnidentified2C;
            mUnidentified0C->SetAssetPosition(position.f.x, mUnidentified30 - offset, position.f.z);
        }
        else
            mUnidentified28 = 0.0f;
    }
    else if (fn_8022FD80(1, false))
    {
        if (mUnidentified34 < mUnidentified38)
        {
            ++mUnidentified34;
            fn_801CBCA0(0x3021A1EE, 0, 0, 1);
            feVector3 position = mUnidentified0C->GetAssetPosition();
            float offset = mUnidentified34 * mUnidentified2C;
            mUnidentified0C->SetAssetPosition(position.f.x, mUnidentified30 - offset, position.f.z);
        }
        else
            mUnidentified28 = 0.0f;
    }
    if (mUnidentified34 <= 0)
        mUnidentified04[0]->SetActiveSlide("unused", true, false);
    if (mUnidentified34 >= mUnidentified38)
        mUnidentified04[1]->SetActiveSlide("unused", true, false);
    if (mUnidentified14[0] && !g_pFEInput->IsPressed(pad, 30, true, 0))
        fn_8022FFA8(event.mIndex, (void*)0);
    if (mUnidentified14[1] && !g_pFEInput->IsPressed(pad, 30, true, 0))
        fn_8022FFA8(event.mIndex, (void*)1);
    if (mUnidentified16[1] && !g_pFEInput->IsPressed(pad, 14, true, 0))
        fn_802301B8(event.mIndex, (void*)1);
    if (mUnidentified16[0] && !g_pFEInput->IsPressed(pad, 13, true, 0))
        fn_802301B8(event.mIndex, (void*)0);
}

void UnidentifiedScrollWidget::fn_802308D0(TLInstance* instance)
{
    mUnidentified00 = (TLComponentInstance*)instance;
    mUnidentified1C = instance->GetAssetPosition();
    TLComponentInstance* up = FEFinder<TLComponentInstance, 4>::Find(mUnidentified00->GetActiveSlide(), "up_arrow");
    mUnidentified04[0] = up == 0 ? &lbl_80580030 : up;
    TLComponentInstance* down = FEFinder<TLComponentInstance, 4>::Find(mUnidentified00->GetActiveSlide(), "down_arrow");
    mUnidentified04[1] = down == 0 ? &lbl_80580030 : down;
    TLInstance* found = FEFinder<TLInstance, 2>::Find(mUnidentified00->GetActiveSlide(), "track", "btn_scroll_minmax");
    mUnidentified0C = found == 0 ? &lbl_80580248 : found;
}

void UnidentifiedScrollWidget::fn_80230B90(int value)
{
    if (value > 0)
    {
        TLInstance* track = FEFinder<TLInstance, 2>::Find(mUnidentified00->GetActiveSlide(), "track", "btn_track ");
        feVector3 trackScale = track->GetScale();
        feVector3 scale = mUnidentified0C->GetScale();
        float distance = 0.63671875 * trackScale.f.y;
        distance = (distance - scale.f.y / 2.0f) * 100.0f;
        mUnidentified2C = distance / value;
        feVector3 position = mUnidentified0C->GetAssetPosition();
        if (mUnidentified48 == 9999.9f)
            mUnidentified48 = position.f.y;
        mUnidentified30 = distance / 2.0f + position.f.y;
        mUnidentified0C->SetAssetPosition(position.f.x, mUnidentified30, position.f.z);
        mUnidentified0C->m_bVisible = true;
    }
    else
    {
        mUnidentified2C = 0.0f;
        mUnidentified30 = 0.0f;
        feVector3 position = mUnidentified0C->GetAssetPosition();
        mUnidentified0C->SetAssetPosition(position.f.x, mUnidentified30, position.f.z);
        mUnidentified0C->m_bVisible = false;
    }
    mUnidentified38 = value;
}

void UnidentifiedScrollWidget::fn_80230DE0(int value)
{
    mUnidentified34 = value;
    feVector3 position = mUnidentified0C->GetAssetPosition();
    float offset = mUnidentified34 * mUnidentified2C;
    mUnidentified0C->SetAssetPosition(position.f.x, mUnidentified30 - offset, position.f.z);
    if (mUnidentified34 <= 0)
        mUnidentified04[0]->SetActiveSlide("unused", true, false);
    else
        mUnidentified04[0]->SetActiveSlide("off", true, false);
    if (mUnidentified34 >= mUnidentified38)
        mUnidentified04[1]->SetActiveSlide("unused", true, false);
    else
        mUnidentified04[1]->SetActiveSlide("off", true, false);
}

void UnidentifiedScrollWidget::fn_80230EE0(int index, void* context)
{
    int direction = (int)context;
    mComponents[direction].mValues[index] = 1;
    int other = -1;
    if (context == 0)
    {
        other = 1;
        if (mUnidentified34 <= 0)
        {
            mUnidentified19 = false;
            return;
        }
    }
    else if (context == (void*)1)
    {
        other = 0;
        if (mUnidentified34 >= mUnidentified38)
        {
            mUnidentified19 = false;
            return;
        }
    }
    if (mUnidentified14[direction])
        mUnidentified04[direction]->SetActiveSlide("slide1", true, false);
    else if (!mUnidentified16[0] && !mUnidentified16[1] && !mUnidentified14[other])
    {
        mUnidentified04[direction]->SetActiveSlide("over", true, false);
        fn_801CBCA0(0x96DEB5C3, 0, 0, 1);
        mComponents[direction].fn_802195B4(index);
    }
    mUnidentified19 = true;
    if (mUnidentified14[direction])
        mUnidentified10[direction] = true;
}
