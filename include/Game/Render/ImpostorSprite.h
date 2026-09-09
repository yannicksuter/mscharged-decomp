#ifndef GAME_RENDER_IMPOSTOR_SPRITE_H
#define GAME_RENDER_IMPOSTOR_SPRITE_H

#include "NL/gl/glTarget.h"
#include "NL/nlMath.h"
#include "types.h"

class GLView;
class GLCompactColourMeshWriter;
class Impostor;
class ImpostorCharacter;
class ImpostorView;
struct ImpostorQuad;

class ImpostorSprite
{
public:
    ImpostorSprite(ImpostorCharacter* character, int texture,
        int budget, int width, int height);
    ~ImpostorSprite();

    void Initialize(const char* name);
    void UpdateView(const nlVector3* direction, const nlVector3* up);
    void UpdateViewport();
    void SuspendCapture();
    void ResumeCapture();
    void CreateRenderTarget(const char* name);
    int Render(GLView* target, Impostor* impostors, bool cached, bool skipCapture);
    void ClearRenderSlots();
    void QueueAllSlots();
    void QueueSlot(int slot);
    unsigned long GetTexture();
    bool AddImpostorSlot(int slot);
    void ClearImpostorSlots();
    void BuildQuad(ImpostorQuad* quad, const Impostor* impostor, const nlVector3* right, const nlVector3* up);
    int CalculateRenderChecksum();

    /* 0x00 */ bool mCaptureSuspended;
    /* 0x01 */ char mName[0x40];
    /* 0x41 */ u8 mUnidentified041[3];
    /* 0x44 */ int mID;
    /* 0x48 */ int mTextureIndex;
    /* 0x4C */ ImpostorCharacter* mCharacter;
    /* 0x50 */ int mWidth;
    /* 0x54 */ int mHeight;
    /* 0x58 */ int* mImpostorSlots;
    /* 0x5C */ int mNumImpostorSlots;
    /* 0x60 */ int mCapacity;
    /* 0x64 */ ImpostorView* mViewInterface;
    /* 0x68 */ GLView* mView;
    /* 0x6C */ GLRenderPair mRenderPair;
    /* 0x74 */ GLCompactColourMeshWriter* mMesh;
    /* 0x78 */ u8 mEnabled;
    /* 0x79 */ u8 mUnidentified079;
    /* 0x7A */ u16 mAngle;
    /* 0x7C */ float mAngleJitter;
    /* 0x80 */ int* mRenderSlots;
    /* 0x84 */ int mNumRenderSlots;
    /* 0x88 */ u8 mUseIntensityAlpha;
    /* 0x89 */ u8 mUnidentified089[3];
}; // size: 0x8C

#endif // GAME_RENDER_IMPOSTOR_SPRITE_H
