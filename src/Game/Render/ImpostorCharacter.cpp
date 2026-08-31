#include "Game/Render/ImpostorCharacter.h"

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

extern "C" int nlSNPrintf(char*, unsigned long, const char*, ...);
extern "C" double floor(double);
extern "C" void fn_802C8280(const char*);
extern "C" void fn_802C8288();
extern "C" void* fn_802CC094();
extern "C" void fn_802D4480(ImpostorSprite_802D4290*, const char*);
extern "C" void fn_802D4484(ImpostorSprite_802D4290*, void*, void*);
extern "C" void fn_802D47F8(ImpostorSprite_802D4290*);
extern "C" void fn_802D4874(ImpostorSprite_802D4290*);
extern "C" void fn_802D4898(ImpostorSprite_802D4290*);
extern "C" void fn_802D50D8(ImpostorSprite_802D4290*, int);
extern "C" void fn_802D5110(ImpostorSprite_802D4290*);
extern "C" ImpostorModel_802DAEE0* fn_802DB0AC(
    ImpostorModel_802DAEE0*, void*);
extern "C" void fn_802DB22C(ImpostorModel_802DAEE0*, float);
extern "C" void fn_802DB26C(ImpostorModel_802DAEE0*);
extern "C" void fn_802DB2B8(ImpostorModel_802DAEE0*, float);
extern "C" void fn_802DB528(ImpostorModel_802DAEE0*, void*, float, int);

// The tweak-value constructor is inlined here, matching the pattern already
// used by Game/InterpreterCore.cpp; the retained out-of-line copy at
// 0x8007571C belongs to a translation unit that does not see this definition.
inline TweakValueImpl_804F4DC8::TweakValueImpl_804F4DC8(float* value)
    : m_pValue(value)
{
}

u32 lbl_806E1F68;

ImpostorCharacter::ImpostorCharacter(const char* name, int budget,
    int numAngles, int numTextures, const ImpostorCharacterParams* params)
    : mNumAngles(numAngles)
    , mNumTextures(numTextures)
    , mUnidentified00C(false)
    , mWidth(0x40)
    , mHeight(0x40)
    , mUnidentified034(false)
    , mUnidentified035(false)
    , mBaseAngle(0)
    , mName(name)
{
    if (params != 0)
    {
        mWidth = params->mWidth;
        mHeight = params->mHeight;
        mUnidentified034 = params->mUnidentified008;
        mUnidentified035 = params->mUnidentified009;
        mBaseAngle = params->mBaseAngle;
    }

    char pathBuffer[0x80];
    nlSNPrintf(pathBuffer, 0x80, "/Render/Impostor/CharacterTweaks/%s", name);

    bool registered = mfScale.fn_802C4FEC(
        "mfScale", 0.0f, pathBuffer, true, 3.0f, 0.001f);
    if (!registered)
    {
        mfScale = mfScale.GetDefaultValue();
    }
    if (!registered)
    {
        mfScale = 1.0f;
    }

    registered = mfCameraLookatZ.fn_802C4FEC(
        "mfCameraLookatZ", 0.0f, pathBuffer, true, 10.0f, 0.01f);
    if (!registered)
    {
        mfCameraLookatZ = mfCameraLookatZ.GetDefaultValue();
    }
    if (!registered)
    {
        mfCameraLookatZ = 1.2f;
    }

    registered = mfCameraDistance.fn_802C4FEC(
        "mfCameraDistance", 0.0f, pathBuffer, true, 40.0f, 0.01f);
    if (!registered)
    {
        mfCameraDistance = mfCameraDistance.GetDefaultValue();
    }
    if (!registered)
    {
        mfCameraDistance = 2.3f;
    }

    fn_802C8280("ImpostorCharacter");

    u16 angleStep = (u16)((int)(65536.0f * (360.0f / (float)mNumAngles)) / 360);
    for (int i = 0; i < mNumTextures; ++i)
    {
        // Retail computes this per-texture stagger value and discards it; the
        // integer conversion it forces is part of the retained object.
        float stagger = nlRandomf(0.0f, 5.0f, &nlDefaultSeed) + (float)i;
        u16 angle = 0;
        for (int j = 0; j < mNumAngles; ++j)
        {
            ImpostorSprite_802D4290* sprite = new ImpostorSprite_802D4290(
                this, i, budget / (mNumAngles * mNumTextures), mWidth,
                mHeight);
            sprite->mUnidentified088 = mUnidentified035;

            char nameBuffer[0x40];
            nlSNPrintf(nameBuffer, 0x40, "Impostor-%s%d", mName,
                i * mNumAngles + j);
            fn_802D4480(sprite, nameBuffer);

            sprite->mAngle = mBaseAngle + angle;
            mSprites.AddEnd(sprite);
            lbl_806E1F68++;
            angle += angleStep;
        }
    }
    fn_802C8288();

    ImpostorManager::GetInstance()->AddCharacter(this);
}

ImpostorCharacter::~ImpostorCharacter()
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    while (it.m_Curr != 0)
    {
        ImpostorSprite_802D4290* sprite = it.m_Curr->entry;
        delete sprite;
        mSprites.Remove(&it);
    }

    mSprites.Clear();
    mSprites.m_Allocator.FreeBlocks();
}


extern "C" u16 fn_802D75AC(u16 target, int count)
{
    u16 step = (u16)((int)(65536.0f * (360.0f / (float)count)) / 360);
    int angle;
    int bestDistance = 0x8000;
    u16 bestAngle = 0;
    angle = 0;
    for (int i = 0; i < count; ++i)
    {
        s16 distance = (s16)((u16)angle - target);
        u16 absDistance = distance < 0 ? -distance : distance;
        if (absDistance < (u16)bestDistance)
        {
            bestDistance = absDistance;
            bestAngle = angle;
        }
        angle += step;
    }
    return bestAngle;
}

void ImpostorCharacter::Acquire(Impostor* impostor)
{
    ImpostorSprite_802D4290* best = 0;
    float pick = floor(nlRandomf(0.0f, (float)mNumTextures, &nlDefaultSeed));
    int index = (int)pick;
    int current = 0;
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorSprite_802D4290* sprite = entry->entry;
        if (sprite->mAngle == impostor->mAngle)
        {
            best = sprite;
            if (index == current)
            {
                break;
            }
            current++;
        }
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }

    fn_802D50D8(best, impostor->mSlot);
    impostor->mpSprite = best;
}

void ImpostorCharacter::ReleaseSprites()
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        fn_802D5110(entry->entry);
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

void ImpostorCharacter::UnidentifiedVirtual2C(void* unidentified0,
    void* unidentified1)
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        fn_802D4484(entry->entry, unidentified0, unidentified1);
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

void ImpostorCharacter::UpdateSprites(int period, int slot)
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    int lastTexture = -1;
    while (entry != 0)
    {
        ImpostorSprite_802D4290* sprite = entry->entry;
        if (sprite->mUnidentified044 % period == slot)
        {
            fn_802D4898(sprite);
            if (sprite->mUnidentified084 > 0 || mUnidentified00C != 0)
            {
                if (lastTexture != sprite->mUnidentified048)
                {
                    UnidentifiedVirtual1C(sprite->mUnidentified048);
                    lastTexture = sprite->mUnidentified048;
                }
                UnidentifiedVirtual20(sprite->mUnidentified068,
                    sprite->mUnidentified048);
            }
        }
        else
        {
            fn_802D4874(sprite);
        }
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

void ImpostorCharacter::EnableSprites(bool enable)
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorSprite_802D4290* sprite = entry->entry;
        sprite->mUnidentified078 = enable;
        fn_802D47F8(sprite);
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

void ImpostorCharacter::RegisterSprites(void* registry)
{
    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        UnidentifiedMesh_802D7AEC* target = entry->entry->mUnidentified068;
        UnidentifiedRegistryNode_802D7AEC* node =
            (UnidentifiedRegistryNode_802D7AEC*)nlMalloc(8, 8, false);
        if (node != 0)
        {
            node->mNext = 0;
            node->mTarget = target;
        }
        node->mNext = 0;

        UnidentifiedRegistry_802D7AEC* list =
            (UnidentifiedRegistry_802D7AEC*)registry;
        if (list->mTail != 0)
        {
            list->mTail->mNext = node;
            list->mTail = node;
        }
        else
        {
            list->mTail = node;
            list->mHead = node;
        }
        target->mUnidentified054 = registry;
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

void ImpostorCharacterImpl_8052E9B8::UnidentifiedVirtual1C(int texture)
{
    fn_802DB26C(mModels[texture]);
}

void ImpostorCharacterImpl_8052E9B8::UnidentifiedVirtual18(int index,
    float phase)
{
    fn_802DB2B8(mModels[index], phase);
}

void ImpostorCharacterImpl_8052E9B8::UnidentifiedVirtual20(void* target,
    int texture)
{
    mModels[texture]->UnidentifiedVirtual0C(target, 0);
}

ImpostorCharacterImpl_8052E9B8::ImpostorCharacterImpl_8052E9B8(
    const char* name, ImpostorModel_802DAEE0* model, void* animations,
    int budget, int numAngles, int numTextures,
    const ImpostorCharacterParams* params)
    : ImpostorCharacter(name, budget, numAngles, numTextures, params)
{
    mNumModels = numTextures;
    mModels = (ImpostorModel_802DAEE0**)nlMalloc(numTextures * 4, 8, false);
    mModels[0] = model;
    for (int i = 1; i < numTextures; ++i)
    {
        mModels[i] = fn_802DB0AC(model, fn_802CC094());
    }
    for (int i = 0; i < numTextures; ++i)
    {
        fn_802DB528(mModels[i], animations, 0.0f, 0);
    }
}

ImpostorCharacterImpl_8052E9B8::~ImpostorCharacterImpl_8052E9B8()
{
    for (int i = 1; i < mNumModels; ++i)
    {
        if (mModels[i] != 0)
        {
            delete mModels[i];
        }
    }
    delete[] mModels;
}




void ImpostorCharacterImpl_8052E9B8::UnidentifiedVirtual24(float dt)
{
    for (int i = 0; i < mNumModels; ++i)
    {
        fn_802DB22C(mModels[i], dt);
    }
}

void ImpostorCharacterImpl_8052E9B8::UnidentifiedVirtual28(float dt,
    int unidentified)
{
    for (int i = 0; i < mNumModels; ++i)
    {
        float value = nlRandomf(0.25f * dt, dt, &nlDefaultSeed);
        fn_802DB528(mModels[i], (void*)unidentified, value, 0);
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
