#include "Game/Render/ImpostorManager.h"

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakConfig.h"
#include "NL/gl/glMemory.h"
#include "Game/TweakValue.h"
#include "NL/gl/glState.h"
#include "NL/nlDebug.h"
#include "Game/TweakValueFloat.h"
#include "Game/TweakValueInt.h"

u8 gDisableImpostorBlending;
u8 gImpostorSpritesInvalid;

static int sImpostorAlphaTestReference = 0x80;
static int sImpostorUpdatePeriod = 1;
static float sDefaultImpostorSizeScale = 1.0f;
extern int sInitialRenderedImpostorCount;

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
    mUnidentified036 = false;
    mUnidentified037 = false;
    mCurrentResource = 0;
    mUnidentified04C = false;
    mLastRenderChecksum = 0;
    mFrameCount = 0;
    mCaptured = false;

    mEnabled = false;
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->EnableSprites(false);
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

void ImpostorManager::Initialize(GLView* registry, int capacity,
    const GLMemoryRequirement* config, int numRequirements, bool flag)
{
    mParentView = registry;
    mImpostors = new (8, false) Impostor[capacity];
    mCapacity = capacity;
    mNumUsed = 0;
    mInitialized = true;
    sfImpostorSizeScale.value = sDefaultImpostorSizeScale;
    gImpostorSpritesInvalid = false;

    for (int i = 0; i < 2; ++i)
    {
        if (config == 0)
        {
            mResources[i] = glCreateResourcePool(
                sImpostorResourceRequirements, 2, "Impostors");
        }
        else
        {
            mResources[i] = glCreateResourcePool(
                config, numRequirements, "Impostors");
        }
        mResourceMarkers[i] = mResources[i]->MarkResource();
    }

    mUnidentified04C = false;
    mCaptured = false;
    mUnidentified037 = false;
    mUnidentified036 = false;
    mUnidentified035 = false;
    mUnidentified059 = flag;
    LoadTweakConfigFile("ini/ImpostorCharacterTweaks.ini",
        "/Render/Impostor/CharacterTweaks", false);
}

void ImpostorManager::InvalidateCapture()
{
    mCaptured = false;
}

void ImpostorManager::ResetImpostors()
{
    for (nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
         it.hasNext(); it.next())
    {
        (*it)->ReleaseSprites();
    }

    for (int i = 0; i < mNumUsed; ++i)
    {
        mImpostors[i].Reset();
        for (nlDLListIterator<ImpostorCharacter*> characters = mCharacters.Begin();
             characters.hasNext(); characters.next())
        {
            for (nlDLListIterator<ImpostorSprite*> sprites =
                     (*characters)->mSprites.Begin();
                 sprites.hasNext(); sprites.next())
            {
                ImpostorSprite* sprite = *sprites;
                sprite->ClearRenderSlots();
                if (gImpostorSpritesInvalid != 0)
                {
                    sprite->QueueAllSlots();
                }
            }
        }
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

    mCharacters.Clear();
    mCharacters.m_Allocator.FreeBlocks();

    BasicSlotPool<DLListEntry<ImpostorCharacter*> >* pool =
        &mCharacters.m_Allocator;
    pool->FreeBlocks();

    glDestroyResourcePool(mResources[0]);
    glDestroyResourcePool(mResources[1]);
    mInitialized = false;
}

void ImpostorManager::ResetSpriteSlots()
{
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        nlDLListIterator<ImpostorSprite*> sprites =
            entry->entry->mSprites.Begin();
        DLListEntry<ImpostorSprite*>* spriteHead = sprites.m_Head;
        DLListEntry<ImpostorSprite*>* spriteEntry = sprites.m_Curr;
        while (spriteEntry != 0)
        {
            ImpostorSprite* sprite = spriteEntry->entry;
            sprite->ClearRenderSlots();
            if (gImpostorSpritesInvalid != 0)
            {
                sprite->QueueAllSlots();
            }
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

void ImpostorManager::Render(void* target, bool skipCapture)
{
    if (mEnabled == 0)
    {
        return;
    }

    if (mUnidentified059 != 0)
    {
        mCaptured = false;
    }

    bool cached = false;
    if (mUnidentified04C != 0 && !skipCapture)
    {
        cached = true;
    }

    if (cached)
    {
        u32 total = 0;
        nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
        DLListEntry<ImpostorCharacter*>* head = it.m_Head;
        DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
        while (entry != 0)
        {
            nlDLListIterator<ImpostorSprite*> sprites =
                entry->entry->mSprites.Begin();
            DLListEntry<ImpostorSprite*>* spriteHead = sprites.m_Head;
            DLListEntry<ImpostorSprite*>* spriteEntry = sprites.m_Curr;
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

    sNumImpostorsRendered.value = sInitialRenderedImpostorCount;
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

    nlDLListIterator<ImpostorCharacter*> drawIt = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* drawHead = drawIt.m_Head;
    DLListEntry<ImpostorCharacter*>* drawEntry = drawIt.m_Curr;
    while (drawEntry != 0)
    {
        ImpostorCharacter* character = drawEntry->entry;
        if (character->mUnidentified034 != 0)
        {
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_DepthWrite, 0);
            u32 blend = 2;
            if (gDisableImpostorBlending != 0)
            {
                blend = 0;
            }
            glSetRasterState(GLS_AlphaBlend, blend);
            glSetCurrentRasterState(glHandleizeRasterState());
        }
        else
        {
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_AlphaBlend, gDisableImpostorBlending == 0);
            glSetCurrentRasterState(glHandleizeRasterState());
        }

        nlDLListIterator<ImpostorSprite*> sprites =
            character->mSprites.Begin();
        DLListEntry<ImpostorSprite*>* spriteHead = sprites.m_Head;
        DLListEntry<ImpostorSprite*>* spriteEntry = sprites.m_Curr;
        while (spriteEntry != 0)
        {
            sNumImpostorsRendered.value += spriteEntry->entry->Render((GLView*)target, mImpostors, cached, skipCapture);
            if (nlDLRingIsEnd(spriteHead, spriteEntry) || spriteEntry == 0)
            {
                spriteEntry = 0;
            }
            else
            {
                spriteEntry = spriteEntry->m_next;
            }
        }
        if (nlDLRingIsEnd(drawHead, drawEntry) || drawEntry == 0)
        {
            drawEntry = 0;
        }
        else
        {
            drawEntry = drawEntry->m_next;
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
    if (character->mUnidentified00C != 0)
    {
        mUnidentified036 = true;
    }
}

void ImpostorManager::UpdateCharacters(float dt, const char* unidentified)
{
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->PlayAnimation(dt, unidentified);
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
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
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

    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        ImpostorCharacter* character = entry->entry;
        if (mUnidentified037 == 0 && character->mUnidentified00C != 0)
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
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
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
    int count = nlDLRingCountElements(mCharacters.m_Head);
    float step = 1.0f / (4.0f * (float)count);

    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
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
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->EnableSprites(enable);
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

void ImpostorManager::SetSpritesInvalid()
{
    gImpostorSpritesInvalid = true;
}

void ImpostorManager::SetUpdatePeriod(int period)
{
    sImpostorUpdatePeriod = period;
}

