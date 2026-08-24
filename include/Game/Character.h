#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "types.h"

class PhysicsCharacterBase;
class cPN_SAnimController;

class cCharacter
{
public:
    virtual ~cCharacter();
    virtual void fn_8001D608();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate(float dt);
    virtual void PreUpdate(float dt);

    void SetAnimState(int animID, bool useBlendTime, float nonDefaultBlendTime,
        bool restartCyclic, bool forceMirrorSwap);
    cPN_SAnimController* NewAnimController(int animID, bool bRestartCyclic,
        bool bForceMirrorSwap,
        void (*funcPlaybackSpeedCallback)(
            unsigned int, cPN_SAnimController*),
        unsigned int nPlaybackSpeedCallbackParam);

protected:
    /* 0x004 */ u8 mUnknown004[0x1C];
    /* 0x020 */ PhysicsCharacterBase* m_pPhysicsCharacter;
    /* 0x024 */ u8 mUnknown024[0x3C];
    /* 0x060 */ unsigned short m_aDesiredFacingDirection;
    /* 0x062 */ unsigned short m_aActualFacingDirection;
    /* 0x064 */ u8 mUnknown064[0x64];
    /* 0x0C8 */ cPN_SAnimController* m_pCurrentAnimController;
    /* 0x0CC */ int m_eAnimID;
    /* 0x0D0 */ u8 mUnknown0D0[0x114];
}; // total size: 0x1E4

#endif // GAME_CHARACTER_H
