#ifndef GAME_FE_INPUT_H
#define GAME_FE_INPUT_H

#include "NL/globalpad.h"
#include "types.h"

class BaseSceneHandler;

enum eFEINPUT_PAD
{
    FE_NO_PAD = -1,
    FE_PAD1_ID = 0,
    FE_PAD2_ID = 1,
    FE_PAD3_ID = 2,
    FE_PAD4_ID = 3,
    FE_PAD5_ID = 4,
    FE_PAD6_ID = 5,
    FE_PAD7_ID = 6,
    FE_PAD8_ID = 7,
    FE_ALL_PADS = 8,
};

struct InputLockEntry
{
    /* 0x00 */ BaseSceneHandler* m_pBaseSceneHandler;
    /* 0x04 */ int m_customID;
}; // size 0x08

struct FEInput
{
    /* 0x00 */ InputLockEntry m_nExclusiveInputSceneHashIDStack[4];
    /* 0x20 */ int m_InputLockDepth;
    /* 0x24 */ unsigned long mUnidentified024[4];
    /* 0x34 */ bool m_bEnableInput[4];
    /* 0x38 */ bool m_bInputAllowed;

    FEInput()
    {
        for (int i = 0; i < 4; i++)
        {
            m_nExclusiveInputSceneHashIDStack[i].m_pBaseSceneHandler = 0;
            m_nExclusiveInputSceneHashIDStack[i].m_customID = -1;
        }
        Reset(true);
    }

    virtual ~FEInput();

    static void Initialize();
    void Reset(bool arg0 = true);
    cGlobalPad* GetGlobalPad(eFEINPUT_PAD pad) const;
    bool IsConnected(eFEINPUT_PAD pad);
    bool IsPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad);
    bool IsAutoPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad);
    bool JustPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad);
    bool PlatJustPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad);
    bool JustReleased(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad);
    void EnableInputIfSceneHasFocus(BaseSceneHandler* pSceneHandler);
    void PushExclusiveInputLock(BaseSceneHandler* pRequestingSceneHandler, int customID);
    void PopExclusiveInputLock(BaseSceneHandler* pRequestingSceneHandler);
    bool HasInputLock(BaseSceneHandler* pRequestingSceneHandler) const;
    void Update(float dt);
    void SetAutoRepeatParams(eFEINPUT_PAD pad, int button, float initialdelay, float repeatrate);
    void EnableAnalogToDPadMapping(eFEINPUT_PAD pad, bool enable);

private:
    bool IsButtonDisabled(eFEINPUT_PAD pad, int button, bool remap) const;
}; // size 0x40

extern FEInput* g_pFEInput;

#endif // GAME_FE_INPUT_H
