#include "Game/Render/ImpostorCharacter.h"

#include "Game/Render/ImpostorManager.h"
#include "Game/Render/tu_802D88F4.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glState.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialShadowTweaks.h"

extern u32 lbl_806E1F34;

static GXMaterialColourTweak_804FC520 siBackgroundRed(
    "siBackgroundRed", "/Render/Impostor/Cluster", 122);
static GXMaterialColourTweak_804FC520 siBackgroundGreen(
    "siBackgroundGreen", "/Render/Impostor/Cluster", 104);
static GXMaterialColourTweak_804FC520 siBackgroundBlue(
    "siBackgroundBlue", "/Render/Impostor/Cluster", 71);
static GXMaterialColourTweak_804FC520 siBackgroundQuadLayer(
    "siBackgroundQuadLayer", lbl_806E1E90, 0);
static GXMaterialFloatTweak_804F4190 sfBackgroundSize(
    "sfBackgroundSize", lbl_806E1E90, 30.0f);
static GXMaterialFloatTweak_804F4190 sfBackgroundY(
    "sfBackgroundY", lbl_806E1E90, 10.0f);

ImpostorCluster_8052EA20::ImpostorCluster_8052EA20(const char* name,
    int budget, const ImpostorCharacterParams* params)
    : ImpostorCharacter(name, budget, 1, 1, params)
    , mUnidentified06C(name)
{
    mUnidentified00C = true;
}

extern "C" unsigned long fn_802D81EC(
    ImpostorCluster_8052EA20* character)
{
    nlDLListIterator<ImpostorSprite_802D4290*> it =
        character->mSprites.Begin();
    return fn_802D50A4(it.m_Curr->entry);
}

void ImpostorCluster_8052EA20::UnidentifiedVirtual2C(
    void*, void*)
{
    const nlVector3 direction = { 0.0f, 1.0f, 0.0f };
    const nlVector3 up = { 0.0f, 0.0f, 1.0f };

    nlDLListIterator<ImpostorSprite_802D4290*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite_802D4290*>* head = it.m_Head;
    DLListEntry<ImpostorSprite_802D4290*>* entry = it.m_Curr;
    while (entry != 0)
    {
        fn_802D4484(entry->entry, &direction, &up);
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

void ImpostorCluster_8052EA20::UnidentifiedVirtual20(void* target, int)
{
    ImpostorManager::GetInstance()->ResetSpriteSlots();
    fn_802D9E34(fn_802D8BB4());
    ImpostorManager::GetInstance()->Render(target, true);

    glSetCurrentTexture(lbl_806E1F34, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    nlMakeRotationMatrixX(matrix, 1.5707964f);
    matrix.m41 = 0.0f;
    matrix.m42 = sfBackgroundY.value;
    matrix.m43 = 0.0f;
    matrix.m44 = 1.0f;

    glQuad3 quad;
    quad.SetupRotatedRectangle(sfBackgroundSize.value,
        sfBackgroundSize.value, matrix, false, false);

    unsigned char red = (unsigned char)siBackgroundRed.value;
    unsigned char green = (unsigned char)siBackgroundGreen.value;
    unsigned char blue = (unsigned char)siBackgroundBlue.value;
    for (int i = 0; i < 4; ++i)
    {
        quad.m_colour[i].c[0] = red;
        quad.m_colour[i].c[1] = green;
        quad.m_colour[i].c[2] = blue;
        quad.m_colour[i].c[3] = 0xFF;
    }
    quad.Attach((eGLView)target, siBackgroundQuadLayer.value);
}

void ImpostorCluster_8052EA20::UnidentifiedVirtual28(float, const char*)
{
}

void ImpostorCluster_8052EA20::UnidentifiedVirtual18(int, float)
{
}

void ImpostorCluster_8052EA20::UnidentifiedVirtual24(float)
{
}

void ImpostorCluster_8052EA20::UnidentifiedVirtual1C(int)
{
}
