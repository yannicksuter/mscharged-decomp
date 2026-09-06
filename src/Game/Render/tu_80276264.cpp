#include "Game/Render/tu_80276264.h"

#include "Game/BasicStadium.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GameInfo.h"
#include "Game/Render/tu_802DCDB4.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

extern BasicStadium* lbl_806E1964;
extern StadiumEntry_80521F68 lbl_80521F68[22];
extern char lbl_8057A698[32];
extern char lbl_8057A6B8[];

bool lbl_806E1970;
void* lbl_806E1974;
unsigned long lbl_806E1978;
bool lbl_806E197C;
void* lbl_806E1980;
unsigned long lbl_806E1984;
bool lbl_806E1988;
void* lbl_806E198C;
unsigned int lbl_806E1990;
void* lbl_806E1994;
bool lbl_806E1998;
void* lbl_806E19A0[2];
StadiumLoadResult_8057A7B8 lbl_8057A7B8[2][22];

extern "C"
{
    bool fn_80340F48(BasicStadium* stadium, void* temporaryData,
        unsigned long temporarySize, void* resourceData,
        unsigned long resourceSize, bool keepResource);
    bool fn_802CDD78(void* data, unsigned long size, void* context, int value);
    void* fn_802C81FC(void* data, unsigned long size,
        unsigned long* numModels, void* context);
    void fn_802C8280(const char* name);
    void fn_802C8288();
    bool fn_80277C38(const StadiumEntry_80521F68* entry);
    bool fn_802763A4(int index, glModel* models, unsigned long numModels);
    bool fn_80276264(glModel* models, unsigned long numModels);
    void fn_80276564(void* data, unsigned long size, void* userData);
    void fn_80276558(void* data, unsigned long size, void* userData);
    void fn_80276354(void* data, unsigned long size, void* userData);
    bool fn_802C8200(const char* path, LoadAsyncCallback callback,
        void* userData, void* context);
    bool fn_802C8204(const char* path, LoadAsyncCallback callback,
        void* userData, void* context);
    bool fn_802B3E94(const char* path, LoadAsyncCallback callback,
        void* userData, unsigned int alignment, int allocType,
        unsigned int chunkSize, void* readBuffer0, void* readBuffer1,
        void* param, unsigned long value, MemoryAllocator* allocator);
}

extern "C" bool fn_80276724()
{
    if (!lbl_806E197C)
    {
        if (lbl_806E1974 != 0)
        {
            lbl_806E197C = true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

extern "C" void fn_80276758()
{
    char buffer[255];
    nlSNPrintf(buffer, sizeof(buffer), "%s/gameworld.tmp.zlib", lbl_8057A6B8);
    ResourceInterface_802CC094* context = fn_802CC094();
    fn_802B3E94(buffer, fn_80276558, 0, 32, AllocateEnd, 0x40000,
        lbl_806E19A0[0], lbl_806E19A0[1], 0, 0, &VirtualAllocator);

    if (!lbl_806E1970)
    {
        char path[128];
        for (int i = 0; i < 22; ++i)
        {
            StadiumEntry_80521F68& entry = lbl_80521F68[i];
            if (fn_80277C38(&entry))
            {
                nlSNPrintf(path, sizeof(path), "%s.rlt", entry.mUnidentified004);
                fn_802C8204(path, fn_80276354, &lbl_8057A7B8[0][i], context);
                nlSNPrintf(path, sizeof(path), "%s.rlg", entry.mUnidentified004);
                fn_802C8200(path, fn_80276354, &lbl_8057A7B8[1][i], context);
            }
        }
    }
}

extern "C" void fn_80276890()
{
    const char* szOriginalTexture = "flag/mario_banners";
    const CharacterInfo& team = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(0)));
    const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(1)));
    const char* szName = team.mName;
    char buffer[64];

    if (NeedsAlternateColour(team, opponent))
    {
        nlSNPrintf(buffer, sizeof(buffer), "%s/%s_banners_alt", szName, szName);
    }
    else
    {
        nlSNPrintf(buffer, sizeof(buffer), "%s/%s_banners", szName, szName);
    }

    for (nlListIterator<ImpostorModel_802DAEE0*> iterator
             = gpWorldNPCManager->mWorldNPCs.Begin();
         iterator.IsValid(); iterator.Next())
    {
        ImpostorModel_802DAEE0* model = iterator.Current();
        model->mOriginalTexture = nlStringHash(szOriginalTexture);
        fn_802DB4EC(model, nlStringHash(buffer));
    }
}

extern "C" void fn_80276990(bool visible)
{
    for (nlListIterator<ImpostorModel_802DAEE0*> iterator
             = gpWorldNPCManager->mWorldNPCs.Begin();
         iterator.IsValid(); iterator.Next())
    {
        iterator.Current()->mVisible = visible;
    }
}

extern "C" bool fn_802769B4()
{
    if (!lbl_806E1988)
    {
        if (lbl_806E1980 != 0)
        {
            glDiscardFrame(1);
            fn_80340F48(lbl_806E1964, lbl_806E1980, lbl_806E1984,
                lbl_806E1974, lbl_806E1978, true);
            nlFree(lbl_806E1980);
            lbl_806E1988 = true;
        }
        else
        {
            return false;
        }
    }

    if (!lbl_806E1970)
    {
        for (int i = 0; i < 22; ++i)
        {
            StadiumLoadResult_8057A7B8& textures = lbl_8057A7B8[0][i];
            StadiumLoadResult_8057A7B8& geometry = lbl_8057A7B8[1][i];
            if (fn_80277C38(&lbl_80521F68[i]) && !textures.mProcessed)
            {
                if (textures.mData != 0 && geometry.mData != 0)
                {
                    fn_802C8280("Tex");
                    fn_802CDD78(textures.mData, textures.mSize, fn_802CC094(), 1);
                    fn_802C8288();
                    nlFree(textures.mData);
                    textures.mData = 0;
                    textures.mProcessed = true;

                    fn_802C8280("Model");
                    unsigned long numModels = 0;
                    glModel* models = (glModel*)fn_802C81FC(
                        geometry.mData, geometry.mSize, &numModels, fn_802CC094());
                    nlFree(geometry.mData);
                    geometry.mData = 0;
                    geometry.mProcessed = true;
                    if (i < 21)
                    {
                        fn_802763A4(i, models, numModels);
                    }
                    else
                    {
                        fn_80276264(models, numModels);
                    }
                    fn_802C8288();
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return true;
}

extern "C" void fn_80276B4C()
{
    if (nlStrLen(lbl_8057A698) != 0)
    {
        char buffer[128];
        nlSNPrintf(buffer, sizeof(buffer),
            "art/effects/%sEffects.bun", lbl_8057A698);
        lbl_806E1990 = nlLoadEntireFileAsync(buffer, fn_80276564,
            &lbl_806E198C, 32, AllocateStart, 0, 0, 0);
        nlSNPrintf(buffer, sizeof(buffer),
            "art/effects/%sEffectsNonRes.bun.zlib", lbl_8057A698);
        lbl_806E1998 = fn_802B3E94(buffer, fn_80276564,
            &lbl_806E1994, 32, AllocateEnd, 0x40000,
            lbl_806E19A0[0], lbl_806E19A0[1], 0, 0, 0);
    }
}

extern "C" bool fn_80276C4C()
{
    if (lbl_806E1990 != 0 || lbl_806E1998)
    {
        if (lbl_806E198C == 0 && lbl_806E1990 != 0)
        {
            return false;
        }
        if (lbl_806E1994 == 0 && lbl_806E1998)
        {
            return false;
        }
        fn_802C8280("Effects");
        fn_802E67E0(lbl_806E198C, lbl_806E1994, fn_802CC094(), 1);
        fn_802C8288();
        lbl_806E198C = 0;
        lbl_806E1990 = 0;
        lbl_806E1994 = 0;
        lbl_806E1998 = false;
    }
    nlFree(lbl_806E19A0[0]);
    lbl_806E19A0[0] = 0;
    lbl_806E19A0[1] = 0;
    return true;
}
