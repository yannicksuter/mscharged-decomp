#include "Game/Render/ImpostorCharacter.h"
#include "NL/gl/glTexture.h"

#include "Game/Render/ImpostorManager.h"
#include "Game/Render/CrowdImpostorManager.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glState.h"
#include "Game/TweakValue.h"
#include "Game/TweakValueInt.h"
#include "Game/TweakValueFloat.h"


static TweakValueInt siBackgroundRed(
    "siBackgroundRed", "/Render/Impostor/Cluster", 122);
static TweakValueInt siBackgroundGreen(
    "siBackgroundGreen", "/Render/Impostor/Cluster", 104);
static TweakValueInt siBackgroundBlue(
    "siBackgroundBlue", "/Render/Impostor/Cluster", 71);
static TweakValueInt siBackgroundQuadLayer(
    "siBackgroundQuadLayer", gLastTweakCategory, 0);
static TweakValueFloat sfBackgroundSize(
    "sfBackgroundSize", gLastTweakCategory, 30.0f);
static TweakValueFloat sfBackgroundY(
    "sfBackgroundY", gLastTweakCategory, 10.0f);

ImpostorCluster::ImpostorCluster(const char* name,
    int budget, const ImpostorCharacterParams* params)
    : ImpostorCharacter(name, budget, 1, 1, params)
    , mName(name)
{
    mUnidentified00C = true;
}

unsigned long ImpostorCluster::GetTexture()
{
    nlDLListIterator<ImpostorSprite*> it =
        mSprites.Begin();
    return it.m_Curr->entry->GetTexture();
}

void ImpostorCluster::UpdateView(
    const nlVector3*, const nlVector3*)
{
    const nlVector3 direction = { 0.0f, 1.0f, 0.0f };
    const nlVector3 up = { 0.0f, 0.0f, 1.0f };

    nlDLListIterator<ImpostorSprite*> it = mSprites.Begin();
    DLListEntry<ImpostorSprite*>* head = it.m_Head;
    DLListEntry<ImpostorSprite*>* entry = it.m_Curr;
    while (entry != 0)
    {
        entry->entry->UpdateView(&direction, &up);
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

void ImpostorCluster::Render(GLView* target, int)
{
    ImpostorManager::GetInstance()->ResetSpriteSlots();
    GetCrowdImpostorManager()->ReleaseCrowdImpostors();
    ImpostorManager::GetInstance()->Render(target, true);

    glSetCurrentTexture(gWhiteTextureID, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMatrix4 matrix;
    nlMakeRotationMatrixX(matrix, 1.5707964f);
    float yValue = sfBackgroundY.value;
    matrix.m41 = 0.0f;
    matrix.m42 = yValue;
    matrix.m43 = 0.0f;
    matrix.m44 = 1.0f;

    glQuad3 quad;
    quad.SetupRotatedRectangle(sfBackgroundSize.value,
        sfBackgroundSize.value, matrix, false, false);

    for (int i = 0; i < 4; ++i)
    {
        quad.m_colour[i].c[0] = (unsigned char)siBackgroundRed.value;
        quad.m_colour[i].c[1] = (unsigned char)siBackgroundGreen.value;
        quad.m_colour[i].c[2] = (unsigned char)siBackgroundBlue.value;
        quad.m_colour[i].c[3] = 0xFF;
    }
    quad.Attach((eGLView)target, siBackgroundQuadLayer.value);
}

void ImpostorCluster::PlayAnimation(float, const char*)
{
}

void ImpostorCluster::SetAnimationTime(int, float)
{
}

void ImpostorCluster::UpdateAnimation(float)
{
}

void ImpostorCluster::EvaluatePose(int)
{
}
