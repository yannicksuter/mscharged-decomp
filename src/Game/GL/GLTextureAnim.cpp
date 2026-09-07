#include "Game/GL/GLTextureAnim.h"

#include "Game/GL/GLInventory.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"

#include <string.h>

extern "C" bool fn_802D3A08(const void* data, unsigned long size)
{
    if (size < sizeof(GLTextureAnim))
    {
        return false;
    }
    return *(const u32*)data == 0x5F6C6669;
}

extern "C" void fn_802D3A34(const void* data, unsigned long size,
    ResourceInterface_802CC094* resource)
{
    GLTextureAnim* anim = (GLTextureAnim*)resource->Allocate(
        sizeof(GLTextureAnim), GLM_Header);
    memcpy(anim, data, sizeof(GLTextureAnim));

    anim->m_frames = (GLAnimTex*)resource->Allocate(
        anim->m_frameCount * sizeof(GLAnimTex), GLM_Header);

    const GLAnimTex* source = (const GLAnimTex*)((const u8*)data
                                                 + sizeof(GLTextureAnim));
    for (int i = 0; i < anim->m_frameCount; ++i)
    {
        GLAnimTex animTex;
        animTex.textureHandle = source->textureHandle;
        animTex.time = source->time;
        ++source;
        anim->SetTexture(i, animTex);
    }

    anim->m_currentFrame = 0;
    for (int i = 0; i < anim->m_frameCount; ++i)
    {
        anim->m_frames[i].textureHandle = fn_802CDFCC(anim->m_frames[i].textureHandle);
    }

    resource->m_inventory->AddTextureAnim(anim->m_unk_0x04, anim);
    glGetTextureManager()->RegisterTextureAnim(anim);
}

extern "C" void fn_802D3B68(GLTextureAnim* anim)
{
    glTextureManager* manager = glGetTextureManager();
    u32 textureHandle = anim->m_unk_0x18;
    manager->mFreeIndices->AddEnd((u16)textureHandle);

    manager->mTextures[textureHandle] = 0;
    anim->m_unk_0x18 = 0xFFFF;
}

GLAnimTex* GLTextureAnim::GetTexture(int frameIndex)
{
    GLAnimTex* textureArray = m_frames;
    if (frameIndex < 0)
    {
        frameIndex = m_currentFrame;
    }
    return textureArray + frameIndex;
}

void GLTextureAnim::Update(float dt)
{
    s32 backwardFrame;
    s32 advancedFrame;
    s32 nextFrame;
    s32 forwardFrame;
    s32 frameCount;

    if (m_isStopped || m_frameCount < 2)
    {
        return;
    }

    m_currentTime += dt;
    GLAnimTex* frame = m_frames + m_currentFrame;

    if (m_currentTime >= frame->time)
    {
        m_currentTime = 0.0f;
        switch (m_mode)
        {
        case 0:
            nextFrame = m_currentFrame + 1;
            m_currentFrame = nextFrame;
            if (nextFrame >= m_frameCount)
            {
                m_currentFrame = 0;
            }
            break;
        case 1:
            if (m_direction > 0)
            {
                forwardFrame = m_currentFrame + 1;
                m_currentFrame = forwardFrame;
                if (forwardFrame >= m_frameCount)
                {
                    m_currentFrame -= 2;
                    m_direction = -1;
                }
            }
            else
            {
                backwardFrame = m_currentFrame - 1;
                m_currentFrame = backwardFrame;
                if (backwardFrame < 0)
                {
                    m_currentFrame = 1;
                    m_direction = 1;
                }
            }
            break;
        case 2:
            advancedFrame = m_currentFrame + 1;
            m_currentFrame = advancedFrame;
            frameCount = m_frameCount;
            if (advancedFrame >= frameCount)
            {
                m_currentFrame = frameCount - 1;
            }
            break;
        }
    }

    glGetTextureManager()->RefreshTextureAnim(this);
}
