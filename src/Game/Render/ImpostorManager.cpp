#include "Game/Render/ImpostorManager.h"

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/gl/glState.h"
#include "NL/nlDebug.h"

extern "C" UnidentifiedView_802CBEC4* fn_802CBFD8(
    const void* config, int layer, const char* name);
extern "C" void fn_802CC02C(UnidentifiedView_802CBEC4* view);
extern "C" void fn_802C6CAC(const char* fileName, const char* category,
    bool unidentified);

u8 lbl_806E1F60;
u8 lbl_806E1F61;

static int lbl_806DF428 = 0x80;
static int lbl_806DF42C = 1;
static float lbl_806DF430 = 1.0f;
extern int lbl_806DF434;

static UnidentifiedViewConfig_8052E828 sImpostorViewConfig = {
    0x00000000,
    0x0000C000,
    0x00000003,
    0x00050000,
};

GXMaterialFloatTweak_804F4190 sfImpostorSizeScale(
    "sfImpostorSizeScale", "Render/Impostor/Visual Tweaks");
GXMaterialColourTweak_804FC520 sNumImpostorsRendered(
    "sNumImpostorsRendered", lbl_806E1E90, 0);

ImpostorManager::ImpostorManager()
    : mEnabled(false)
    , mImpostors(0)
    , mNumUsed(0)
    , mCapacity(0)
    , mUnidentified010(0)
{
    mpRegistry = 0;
    mInitialized = false;
    mUnidentified035 = false;
    mUnidentified036 = false;
    mUnidentified037 = false;
    mCurrentView = 0;
    mUnidentified04C = false;
    mLastSpriteCount = 0;
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

void ImpostorManager::Initialize(void* registry, int capacity,
    const UnidentifiedViewConfig_8052E828* config, int layer, bool flag)
{
    mpRegistry = registry;
    mImpostors = new (8, false) Impostor[capacity];
    mCapacity = capacity;
    mNumUsed = 0;
    mInitialized = true;
    sfImpostorSizeScale.value = lbl_806DF430;
    lbl_806E1F61 = false;

    for (int i = 0; i < 2; ++i)
    {
        if (config == 0)
        {
            mViews[i] = fn_802CBFD8(&sImpostorViewConfig, 2, "Impostors");
        }
        else
        {
            mViews[i] = fn_802CBFD8(config, layer, "Impostors");
        }
        mCameras[i] = mViews[i]->UnidentifiedVirtual0C();
    }

    mUnidentified04C = false;
    mCaptured = false;
    mUnidentified037 = false;
    mUnidentified036 = false;
    mUnidentified035 = false;
    mUnidentified059 = flag;
    fn_802C6CAC("ini/ImpostorCharacterTweaks.ini",
        "/Render/Impostor/CharacterTweaks", false);
}

void ImpostorManager::InvalidateCapture()
{
    mCaptured = false;
}

void ImpostorManager::ResetImpostors()
{
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->ReleaseSprites();
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }

    for (int i = 0; i < mNumUsed; ++i)
    {
        mImpostors[i].Reset();

        nlDLListIterator<ImpostorCharacter*> characters = mCharacters.Begin();
        DLListEntry<ImpostorCharacter*>* charHead = characters.m_Head;
        DLListEntry<ImpostorCharacter*>* charEntry = characters.m_Curr;
        while (charEntry != 0)
        {
            nlDLListIterator<ImpostorSprite_802D4290*> sprites =
                charEntry->entry->mSprites.Begin();
            DLListEntry<ImpostorSprite_802D4290*>* spriteHead = sprites.m_Head;
            DLListEntry<ImpostorSprite_802D4290*>* spriteEntry = sprites.m_Curr;
            while (spriteEntry != 0)
            {
                ImpostorSprite_802D4290* sprite = spriteEntry->entry;
                fn_802D5034(sprite);
                if (lbl_806E1F61 != 0)
                {
                    fn_802D5040(sprite);
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
            if (nlDLRingIsEnd(charHead, charEntry) || charEntry == 0)
            {
                charEntry = 0;
            }
            else
            {
                charEntry = charEntry->m_next;
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

    fn_802CC02C(mViews[0]);
    fn_802CC02C(mViews[1]);
    mInitialized = false;
}

void ImpostorManager::ResetSpriteSlots()
{
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        nlDLListIterator<ImpostorSprite_802D4290*> sprites =
            entry->entry->mSprites.Begin();
        DLListEntry<ImpostorSprite_802D4290*>* spriteHead = sprites.m_Head;
        DLListEntry<ImpostorSprite_802D4290*>* spriteEntry = sprites.m_Curr;
        while (spriteEntry != 0)
        {
            ImpostorSprite_802D4290* sprite = spriteEntry->entry;
            fn_802D5034(sprite);
            if (lbl_806E1F61 != 0)
            {
                fn_802D5040(sprite);
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
            nlDLListIterator<ImpostorSprite_802D4290*> sprites =
                entry->entry->mSprites.Begin();
            DLListEntry<ImpostorSprite_802D4290*>* spriteHead = sprites.m_Head;
            DLListEntry<ImpostorSprite_802D4290*>* spriteEntry = sprites.m_Curr;
            while (spriteEntry != 0)
            {
                total += fn_802D536C(spriteEntry->entry);
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

        if (total != mLastSpriteCount)
        {
            cached = false;
            mLastSpriteCount = total;
        }
    }

    if (!cached && !skipCapture && mCaptured == 0)
    {
        ImpostorManager* instance = GetInstance();
        instance->mCurrentView = (instance->mCurrentView + 1) % 2;
        instance->mViews[instance->mCurrentView]->UnidentifiedVirtual10(
            instance->mCameras[instance->mCurrentView]);
        instance->mCaptured = true;
        instance->mCameras[instance->mCurrentView] =
            instance->mViews[instance->mCurrentView]->UnidentifiedVirtual0C();
        u32* camera = (u32*)instance->mCameras[instance->mCurrentView];
        if (camera[0] != 0 || camera[1] != 0)
        {
            nlBreak();
        }
    }

    sNumImpostorsRendered.value = lbl_806DF434;
    glSetDefaultState(true);
    glSetRasterState(GLS_DepthWrite, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthTest, 1);
    glSetRasterState(GLS_AlphaBlend, lbl_806E1F60 == 0);
    if (lbl_806E1F60 == 0)
    {
        glSetRasterState(GLS_AlphaTestRef, lbl_806DF428);
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
            if (lbl_806E1F60 != 0)
            {
                blend = 0;
            }
            glSetRasterState(GLS_AlphaBlend, blend);
            glSetCurrentRasterState(glHandleizeRasterState());
        }
        else
        {
            glSetRasterState(GLS_DepthTest, 1);
            glSetRasterState(GLS_AlphaBlend, lbl_806E1F60 == 0);
            glSetCurrentRasterState(glHandleizeRasterState());
        }

        nlDLListIterator<ImpostorSprite_802D4290*> sprites =
            character->mSprites.Begin();
        DLListEntry<ImpostorSprite_802D4290*>* spriteHead = sprites.m_Head;
        DLListEntry<ImpostorSprite_802D4290*>* spriteEntry = sprites.m_Curr;
        while (spriteEntry != 0)
        {
            sNumImpostorsRendered.value += fn_802D4AEC(spriteEntry->entry,
                (GLView*)target, mImpostors, cached, skipCapture);
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
    character->RegisterSprites(mpRegistry);
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
        entry->entry->UnidentifiedVirtual28(dt, unidentified);
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
        entry->entry->UnidentifiedVirtual24(dt);
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
        character->UpdateSprites(lbl_806DF42C, sUpdateSlot);
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }

    sUpdateSlot = (sUpdateSlot + 1) % lbl_806DF42C;
}

float ImpostorManager::GetImpostorSizeScale()
{
    return sfImpostorSizeScale.value;
}

void ImpostorManager::SetImpostorSizeScale(float scale)
{
    sfImpostorSizeScale.value = scale;
}

void ImpostorManager::UpdatePositions(void* unidentified0, void* unidentified1)
{
    nlDLListIterator<ImpostorCharacter*> it = mCharacters.Begin();
    DLListEntry<ImpostorCharacter*>* head = it.m_Head;
    DLListEntry<ImpostorCharacter*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->UnidentifiedVirtual2C(unidentified0, unidentified1);
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
            character->UnidentifiedVirtual18(i, value);
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
    lbl_806E1F61 = true;
}

void ImpostorManager::SetUpdatePeriod(int period)
{
    lbl_806DF42C = period;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
