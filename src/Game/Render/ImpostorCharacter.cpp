#include "NL/nlPrint.h"
#include "Game/Render/ImpostorCharacter.h"

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

#include <math.h>

u32 gNumImpostorSpritesCreated;

ImpostorCharacter::ImpostorCharacter(const char* name, int budget,
    int numAngles, int numTextures, const ImpostorCharacterParams* params)
    : mNumAngles(numAngles)
    , mNumTextures(numTextures)
    , mUnidentified00C(false)
    , mWidth(0x40)
    , mHeight(0x40)
    , mUnidentified034(false)
    , mUseIntensityAlpha(false)
    , mBaseAngle(0)
    , mName(name)
{
    if (params != 0)
    {
        mWidth = params->mWidth;
        mHeight = params->mHeight;
        mUnidentified034 = params->mUnidentified008;
        mUseIntensityAlpha = params->mUseIntensityAlpha;
        mBaseAngle = params->mBaseAngle;
    }

    char pathBuffer[0x80];
    nlSNPrintf(pathBuffer, 0x80, "/Render/Impostor/CharacterTweaks/%s", name);

    mfScale.BindWithDefault("mfScale", 1.0f, pathBuffer, true, 0.0f, 3.0f, 0.001f);
    mfCameraLookatZ.BindWithDefault("mfCameraLookatZ", 1.2f, pathBuffer, true, 0.0f, 10.0f, 0.01f);
    mfCameraDistance.BindWithDefault("mfCameraDistance", 2.3f, pathBuffer, true, 0.0f, 40.0f, 0.01f);

    glBeginResource("ImpostorCharacter");

    u16 angleStep = (u16)((int)(65536.0f * (360.0f / (float)mNumAngles)) / 360);
    for (int i = 0; i < mNumTextures; ++i)
    {
        // Retail computes this per-texture stagger value and discards it; the
        // integer conversion it forces is part of the retained object.
        float stagger = nlRandomf(0.0f, 5.0f, &nlDefaultSeed) + (float)i;
        u16 angle = 0;
        for (int j = 0; j < mNumAngles; ++j)
        {
            ImpostorSprite* sprite = new ImpostorSprite(
                this, i, budget / (mNumAngles * mNumTextures), mWidth, mHeight);
            sprite->mUseIntensityAlpha = mUseIntensityAlpha;

            char nameBuffer[0x40];
            nlSNPrintf(nameBuffer, 0x40, "Impostor-%s%d", mName, i * mNumAngles + j);
            sprite->Initialize(nameBuffer);

            sprite->mAngle = mBaseAngle + angle;
            mSprites.AddEnd(sprite);
            gNumImpostorSpritesCreated++;
            angle += angleStep;
        }
    }
    glEndResource();

    ImpostorManager::GetInstance()->AddCharacter(this);
}

ImpostorCharacter::~ImpostorCharacter()
{
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    while (it.m_Curr != 0)
    {
        ImpostorSprite* sprite = it.m_Curr->entry;
        delete sprite;
        mSprites.Remove(&it);
    }

    mSprites.Clear();
    mSprites.m_Allocator.FreeBlocks();
}

u16 QuantizeImpostorAngle(u16 target, int count)
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
    ImpostorSprite* best = 0;
    float pick = floor(nlRandomf(0.0f, (float)mNumTextures, &nlDefaultSeed));
    DLListEntry<ImpostorSprite*>* head;
    DLListEntry<ImpostorSprite*>* entry;
    int index = (int)pick;
    int current = 0;
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    head = it.m_Head;
    entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorSprite* sprite = entry->entry;
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

    best->AddImpostorSlot(impostor->mSlot);
    impostor->mpSprite = best;
}

void ImpostorCharacter::ReleaseSprites()
{
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite*>* head = it.m_Head;
    DLListEntry<ImpostorSprite*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->ClearImpostorSlots();
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

void ImpostorCharacter::UpdateView(const nlVector3* direction,
    const nlVector3* up)
{
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite*>* head = it.m_Head;
    DLListEntry<ImpostorSprite*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->UpdateView(direction, up);
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
    DLListEntry<ImpostorSprite*>* head;
    DLListEntry<ImpostorSprite*>* entry;
    int lastTexture = -1;

    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    head = it.m_Head;
    entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorSprite* sprite = entry->entry;
        if (sprite->mID % period == slot)
        {
            sprite->ResumeCapture();
            if (sprite->mNumRenderSlots > 0 || mUnidentified00C != 0)
            {
                if (lastTexture != sprite->mTextureIndex)
                {
                    EvaluatePose(sprite->mTextureIndex);
                    lastTexture = sprite->mTextureIndex;
                }
                Render(sprite->mView,
                    sprite->mTextureIndex);
            }
        }
        else
        {
            sprite->SuspendCapture();
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
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite*>* head = it.m_Head;
    DLListEntry<ImpostorSprite*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorSprite* sprite = entry->entry;
        sprite->mEnabled = enable;
        sprite->UpdateViewport();
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

void ImpostorCharacter::RegisterSprites(GLView* registry)
{
    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    GLView* target;
    DLListEntry<ImpostorSprite*>* head = it.m_Head;
    DLListEntry<ImpostorSprite*>* entry = it.m_Curr;
    while (entry != 0)
    {
        target = entry->entry->mView;
        registry->m_Children.AddEnd(target);
        target->m_Parent = registry;
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

void AnimatedImpostorCharacter::EvaluatePose(int texture)
{
    mModels[texture]->EvaluatePose();
}

void AnimatedImpostorCharacter::SetAnimationTime(int index,
    float phase)
{
    mModels[index]->SetAnimationTime(phase);
}

void AnimatedImpostorCharacter::Render(GLView* target,
    int texture)
{
    mModels[texture]->Render(target, 0);
}

AnimatedImpostorCharacter::AnimatedImpostorCharacter(
    const char* name, ImpostorModel* model, void* animations,
    int budget, int numAngles, int numTextures,
    const ImpostorCharacterParams* params)
    : ImpostorCharacter(name, budget, numAngles, numTextures, params)
{
    mNumModels = numTextures;
    mModels = (ImpostorModel**)nlMalloc(numTextures * 4, 8, false);
    mModels[0] = model;
    for (int i = 1; i < numTextures; ++i)
    {
        mModels[i] = model->Clone(glGetCurrentResourcePool());
    }
    for (int i = 0; i < numTextures; ++i)
    {
        mModels[i]->PlayAnimation((const char*)animations, 0.0f, PM_CYCLIC);
    }
}

AnimatedImpostorCharacter::~AnimatedImpostorCharacter()
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

void AnimatedImpostorCharacter::UpdateAnimation(float dt)
{
    for (int i = 0; i < mNumModels; ++i)
    {
        mModels[i]->UpdateAnimation(dt);
    }
}

void AnimatedImpostorCharacter::PlayAnimation(float dt,
    const char* unidentified)
{
    for (int i = 0; i < mNumModels; ++i)
    {
        float value = nlRandomf(0.25f * dt, dt, &nlDefaultSeed);
        mModels[i]->PlayAnimation(unidentified, value, PM_CYCLIC);
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
