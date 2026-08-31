#ifndef _REPLAYMANAGER_H_
#define _REPLAYMANAGER_H_

#include "Game/Camera/FollowCam.h"
#include "Game/RenderSnapshot.h"
#include "Game/Replay.h"

class ReplayManager
{
    ReplayManager();
    void SwapPreviousAndCurrent();
    void DoPotentialDebugReplay(float& deltaTime);
    void DoPotentialAutoReplay(float deltaTime);

public:
    static ReplayManager* Instance();
    void Initialize();
    void InitializeSnapshots();
    void fn_801895B0();
    void fn_801895C0();
    void fn_801895D0();
    void fn_801895E0();
    void fn_80189610();
    void fn_80189620();
    void Uninitialize();
    void GrabSnapshot();
    RenderSnapshot& GetMutableRenderSnapshot();
    void Flush();
    void ResetSnapshots();
    void PrepareForRecording();
    void SetCurrentTime(float time);
    void RenderSnapshotAt(float deltaTime);

    /* 0x0000 */ RenderSnapshot mSnapshots[3];
    /* 0x7554 */ RenderSnapshot* mCurrent;
    /* 0x7558 */ RenderSnapshot* mPrevious;
    /* 0x755C */ RenderSnapshot* mRender;
    /* 0x7560 */ cFollowCamera mDebugCamera;
    /* 0x7604 */ cBaseCamera* mUnidentified7604;
    /* 0x7608 */ u32 mEvents;
    /* 0x760C */ f32 mSpeed;
    /* 0x7610 */ f32 mSpeedUp;
    /* 0x7614 */ f32 mDeltaTime;
    /* 0x7618 */ f32 mTime;
    /* 0x761C */ f32 mBlend[3];
    /* 0x7628 */ Replay* mReplay;
    /* 0x762C */ u8* mMemory;
}; // total size: 0x7630

#endif // _REPLAYMANAGER_H_
