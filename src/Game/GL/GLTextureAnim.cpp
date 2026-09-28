#include "Game/GL/GLTextureAnim.h"

#include "Game/GL/GLInventory.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"

#include <string.h>

bool glIsTextureAnim(const void* data, unsigned long size)
{
    if (size < sizeof(GLTextureAnim))
    {
        return false;
    }
    return *(const u32*)data == 0x5F6C6669;
}

void glAddTextureAnim(void* data, unsigned long size,
    GLResourcePool* resource)
{
    int offset;
    GLTextureAnim* anim = (GLTextureAnim*)resource->Allocate(
        sizeof(GLTextureAnim), GLM_Header);
    memcpy(anim, data, sizeof(GLTextureAnim));

    anim->m_pAnimTex = (GLAnimTex*)resource->Allocate(
        anim->m_nNumTextures * sizeof(GLAnimTex), GLM_Header);

    const GLAnimTex* source = (const GLAnimTex*)((u8*)data
                                                 + sizeof(GLTextureAnim));
    int i;
    for (i = 0; i < anim->m_nNumTextures; ++i)
    {
        GLAnimTex animTex;
        animTex.m_TexHandle = source[i].m_TexHandle;
        animTex.m_fTime = source[i].m_fTime;
        anim->SetTexture(i, animTex);
    }

    anim->m_nFrame = 0;
    for (i = 0, offset = 0; i < anim->m_nNumTextures;
         ++i, offset += sizeof(GLAnimTex))
    {
        unsigned long texture = glGetTextureIndex(
            ((GLAnimTex*)((u8*)anim->m_pAnimTex + offset))->m_TexHandle);
        ((GLAnimTex*)((u8*)anim->m_pAnimTex + offset))->m_TexHandle = texture;
    }

    resource->m_inventory->AddTextureAnim(anim->m_uHashID, anim);
    glGetTextureManager()->RegisterTextureAnim(anim);
}

void glReleaseTextureAnim(GLTextureAnim* anim)
{
    glTextureManager* manager = glGetTextureManager();
    u32 textureHandle = anim->GetTextureIndex();
    manager->mFreeIndices->AddEnd((u16)textureHandle);

    manager->mTextures[textureHandle] = 0;
    anim->m_textureIndex = 0xFFFF;
}

GLAnimTex& GLTextureAnim::GetTexture(int index)
{
    GLAnimTex* textureArray = m_pAnimTex;
    if (index < 0)
    {
        index = m_nFrame;
    }
    return textureArray[index];
}

void GLTextureAnim::Update(float dt)
{
    s32 backwardFrame;
    s32 advancedFrame;
    s32 nextFrame;
    s32 forwardFrame;
    s32 frameCount;

    if (m_bPaused || m_nNumTextures < 2)
    {
        return;
    }

    m_fTime += dt;
    GLAnimTex* frame = m_pAnimTex + m_nFrame;

    if (m_fTime >= frame->m_fTime)
    {
        m_fTime = 0.0f;
        switch (m_ePlayMode)
        {
        case GLAnimMode_Loop:
            nextFrame = m_nFrame + 1;
            m_nFrame = nextFrame;
            if (nextFrame >= m_nNumTextures)
            {
                m_nFrame = 0;
            }
            break;
        case GLAnimMode_PingPong:
            if (m_nPlayDir > 0)
            {
                forwardFrame = m_nFrame + 1;
                m_nFrame = forwardFrame;
                if (forwardFrame >= m_nNumTextures)
                {
                    m_nFrame -= 2;
                    m_nPlayDir = -1;
                }
            }
            else
            {
                backwardFrame = m_nFrame - 1;
                m_nFrame = backwardFrame;
                if (backwardFrame < 0)
                {
                    m_nFrame = 1;
                    m_nPlayDir = 1;
                }
            }
            break;
        case GLAnimMode_Hold:
            advancedFrame = m_nFrame + 1;
            m_nFrame = advancedFrame;
            frameCount = m_nNumTextures;
            if (advancedFrame >= frameCount)
            {
                m_nFrame = frameCount - 1;
            }
            break;
        }
    }

    glGetTextureManager()->RefreshTextureAnim(this);
}
