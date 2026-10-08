#include "NL/nlDLListContainer.inl"
#include "Game/Render/ImpostorManager.h"

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/CharacterEffects.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakConfig.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/nlDebug.h"
#include "Game/TweakValueFloat.h"
#include "Game/TweakValueInt.h"

u8 gDisableImpostorBlending;
u8 gImpostorSpritesInvalid;

static int sImpostorAlphaTestReference = 0x80;
static int sImpostorUpdatePeriod = 1;
static float sDefaultImpostorSizeScale = 1.0f;

static GLMemoryRequirement sImpostorResourceRequirements[2] = {
    { GLM_Header, 0xC000 },
    { GLM_VertexData, 0x50000 },
};

TweakValueFloat sfImpostorSizeScale(
    "sfImpostorSizeScale", "Render/Impostor/Visual Tweaks");
TweakValueInt sNumImpostorsRendered(
    "sNumImpostorsRendered", gLastTweakCategory, 0);

ImpostorManager::ImpostorManager()
    : mEnabled(false)
    , mImpostors(0)
    , mNumUsed(0)
    , mCapacity(0)
    , mUnidentified010(0)
{
    mParentView = 0;
    mInitialized = false;
    mUnidentified035 = false;
    mHasClusters = false;
    mUpdateClusters = false;
    mCurrentResource = 0;
    mUseRenderCache = false;
    mLastRenderChecksum = 0;
    mFrameCount = 0;
    mCaptured = false;

    SetEnabled(false);
}

void ImpostorManager::Initialize(GLView* parentView, int capacity,
    const GLMemoryRequirement* requirements, int numRequirements,
    bool invalidateCaptureOnRender)
{
    mParentView = parentView;
    mImpostors = new (8, false) Impostor[capacity];
    mCapacity = capacity;
    mNumUsed = 0;
    mInitialized = true;
    sfImpostorSizeScale.value = sDefaultImpostorSizeScale;
    gImpostorSpritesInvalid = false;

    for (int i = 0; i < 2; ++i)
    {
        if (requirements == 0)
        {
            mResources[i] = glCreateResourcePool(
                sImpostorResourceRequirements, 2, "Impostors");
        }
        else
        {
            mResources[i] = glCreateResourcePool(
                requirements, numRequirements, "Impostors");
        }
        mResourceMarkers[i] = mResources[i]->MarkResource();
    }

    mUseRenderCache = false;
    mCaptured = false;
    mUpdateClusters = false;
    mHasClusters = false;
    mUnidentified035 = false;
    mInvalidateCaptureOnRender = invalidateCaptureOnRender;
    LoadTweakConfigFile("ini/ImpostorCharacterTweaks.ini",
        "/Render/Impostor/CharacterTweaks", false);
}

void ImpostorManager::InvalidateCapture()
{
    mCaptured = false;
}

void ImpostorManager::ResetImpostors()
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    while (it.hasNext())
    {
        (*it)->ReleaseSprites();
        it.Step();
    }

    for (int i = 0; i < mNumUsed; ++i)
    {
        mImpostors[i].Reset();
        ResetSpriteSlots();
    }

    mNumUsed = 0;
}

void ImpostorManager::Uninitialize()
{
    if (mImpostors != 0)
    {
        delete[] mImpostors;
        mImpostors = 0;
    }

    mCharacters.Free();

    BasicSlotPool<DLListEntry<ImpostorCharacter*> >* pool =
        &mCharacters.m_Allocator;
    pool->FreeBlocks();

    glDestroyResourcePool(mResources[0]);
    glDestroyResourcePool(mResources[1]);
    mInitialized = false;
}

void ImpostorManager::ResetSpriteSlots()
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    while (it.hasNext())
    {
        nlDLListIterator<ImpostorSprite*> sprites;
        sprites = (*it)->mSprites.Begin();
        while (sprites.hasNext())
        {
            ImpostorSprite* sprite = *sprites;
            sprite->ClearRenderSlots();
            if (gImpostorSpritesInvalid != 0)
            {
                sprite->QueueAllSlots();
            }
            sprites.Step();
        }
        it.Step();
    }
}

ImpostorManager* ImpostorManager::GetInstance()
{
    static ImpostorManager sInstance;
    return &sInstance;
}

ImpostorManager::~ImpostorManager()
{
}

int ImpostorManager::GetNumImpostors()
{
    return mNumUsed;
}

static inline u32 AccumulateRenderChecksums(
    const nlDLListSlotPool<ImpostorCharacter*>& characters, u32 total)
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = characters.Begin();
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    while (entry != 0)
    {
        nlDLListIterator<ImpostorSprite*> sprites;
        sprites = entry->entry->mSprites.Begin();
        DLListEntry<ImpostorSprite*>* spriteEntry = sprites.m_Curr;
        DLListEntry<ImpostorSprite*>* spriteHead = sprites.m_Head;
        while (spriteEntry != 0)
        {
            total += spriteEntry->entry->CalculateRenderChecksum();
            if (nlDLRingIsEnd(spriteHead, spriteEntry) || spriteEntry == 0)
            {
                spriteEntry = 0;
            }
            else
            {
                spriteEntry = spriteEntry->m_next;
            }
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
    return total;
}

void ImpostorManager::Render(GLView* target, bool skipCapture)
{
    u32 total;
    if (mEnabled == 0)
    {
        return;
    }

    if (mInvalidateCaptureOnRender != 0)
    {
        mCaptured = false;
    }

    bool cached = false;
    if (mUseRenderCache != 0 && !skipCapture)
    {
        cached = true;
    }

    if (cached)
    {
        total = AccumulateRenderChecksums(mCharacters, 0);
        if (total != mLastRenderChecksum)
        {
            cached = false;
            mLastRenderChecksum = total;
        }
    }

    if (!cached && !skipCapture && mCaptured == 0)
    {
        ImpostorManager* instance = GetInstance();
        instance->mCurrentResource = (instance->mCurrentResource + 1) % 2;
        instance->mResources[instance->mCurrentResource]->ReleaseResource(
            instance->mResourceMarkers[instance->mCurrentResource]);
        instance->mCaptured = true;
        instance->mResourceMarkers[instance->mCurrentResource] =
            instance->mResources[instance->mCurrentResource]->MarkResource();
        u32* marker = (u32*)instance->mResourceMarkers[instance->mCurrentResource];
        if (marker[0] != 0 || marker[1] != 0)
        {
            nlBreak();
        }
    }

    sNumImpostorsRendered = 0;
    glSetDefaultState(true);
    glSetRasterState(GLS_DepthWrite, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthTest, 1);
    glSetRasterState(GLS_AlphaBlend, gDisableImpostorBlending == 0);
    if (gDisableImpostorBlending == 0)
    {
        glSetRasterState(GLS_AlphaTestRef, sImpostorAlphaTestReference);
        glSetRasterState(GLS_AlphaTest, 1);
    }
    glSetCurrentRasterState(glHandleizeRasterState());

    int rendered;
    DLListEntry<ImpostorSprite*>* spriteEntry;
    ImpostorCharacter* character;
    nlDLListIterator<ImpostorCharacter*> drawIt;
    drawIt = mCharacters.Begin();
    for (; drawIt.hasNext(); drawIt.next())
    {
        character = *drawIt;
        if (character->mUseAdditiveBlend != 0)
        {
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_DepthWrite, 0);
            glSetRasterState(GLS_AlphaBlend,
                gDisableImpostorBlending != 0 ? GLB_None : GLB_Additive);
            glSetCurrentRasterState(glHandleizeRasterState());
        }
        else
        {
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_AlphaBlend, gDisableImpostorBlending == 0);
            glSetCurrentRasterState(glHandleizeRasterState());
        }

        nlDLListIterator<ImpostorSprite*> sprites;
        sprites = character->mSprites.Begin();
        DLListEntry<ImpostorSprite*>* spriteHead = sprites.m_Head;
        spriteEntry = sprites.m_Curr;
        while (spriteEntry != 0)
        {
            rendered = sNumImpostorsRendered.mValue;
            sNumImpostorsRendered.mValue = rendered
                + spriteEntry->entry->Render(target, mImpostors, cached, skipCapture);
            if (nlDLRingIsEnd(spriteHead, spriteEntry) || spriteEntry == 0)
            {
                spriteEntry = 0;
            }
            else
            {
                spriteEntry = spriteEntry->m_next;
            }
        }
    }

    mFrameCount++;
}

Impostor* ImpostorManager::AllocImpostor(int* outIndex)
{
    int index = mNumUsed;
    if (index == mCapacity)
    {
        return 0;
    }

    Impostor* impostor = &mImpostors[index];
    impostor->mSlot = index;
    mNumUsed++;
    *outIndex = index;
    return impostor;
}

void ImpostorManager::AddCharacter(ImpostorCharacter* character)
{
    mCharacters.AddEnd(character);
    character->RegisterSprites(mParentView);
    if (character->mIsCluster != 0)
    {
        mHasClusters = true;
    }
}

void ImpostorManager::UpdateCharacters(float blendTime, const char* name)
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->PlayAnimation(blendTime, name);
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

void ImpostorManager::UpdateAnimations(float dt)
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->UpdateAnimation(dt);
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

void ImpostorManager::UpdateSprites()
{
    static int sUpdateSlot;

    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorCharacter* character = entry->entry;
        if (mUpdateClusters == 0 && character->mIsCluster != 0)
        {
            break;
        }
        character->UpdateSprites(sImpostorUpdatePeriod, sUpdateSlot);
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }

    sUpdateSlot = (sUpdateSlot + 1) % sImpostorUpdatePeriod;
}

float ImpostorManager::GetImpostorSizeScale()
{
    return sfImpostorSizeScale.value;
}

void ImpostorManager::SetImpostorSizeScale(float scale)
{
    sfImpostorSizeScale.value = scale;
}

void ImpostorManager::UpdatePositions(const nlVector3* direction, const nlVector3* up)
{
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
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

void ImpostorManager::StaggerAnimations()
{
    float phase = 0.0f;
    int count = mCharacters.CountElements();
    float step = 1.0f / (4.0f * (float)count);

    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorCharacter* character = entry->entry;
        int numTextures = character->mNumTextures;
        for (int i = 0; i < numTextures; ++i)
        {
            float value = (float)i / (float)(numTextures * 4);
            value += phase;
            while (value > 1.0f)
            {
                value -= 1.0f;
            }
            character->SetAnimationTime(i, value);
        }
        phase += step;
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

void ImpostorManager::SetEnabled(bool enable)
{
    mEnabled = enable;
    nlDLListIterator<ImpostorCharacter*> it;
    it = mCharacters.Begin();
    while (it.hasNext())
    {
        (*it)->EnableSprites(enable);
        it.Step();
    }
}

void ImpostorManager::SetSpritesInvalid()
{
    gImpostorSpritesInvalid = true;
}

void ImpostorManager::SetUpdatePeriod(int period)
{
    sImpostorUpdatePeriod = period;
}
