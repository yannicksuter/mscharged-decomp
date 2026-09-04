#ifndef GAME_EFFECTS_EMISSION_MANAGER_H
#define GAME_EFFECTS_EMISSION_MANAGER_H

#include "NL/nlDLListContainer.h"
#include "NL/nlFunction.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "types.h"

class EmissionController;
class EffectsGroup;
class LoadFrame;
class Particle;
class SaveFrame;
class TweakValueIntImpl_804FD898;

class EffectsLight
{
public:
    /* 0x00 */ nlVector3 m_v3Position;
    /* 0x0C */ float m_fRadius;
    /* 0x10 */ nlColour m_Colour;
}; // size: 0x14

struct EmissionResourceStats
{
    EmissionResourceStats();

    /* 0x00 */ TweakValueIntImpl_804FD898* mCount;
    /* 0x04 */ TweakValueIntImpl_804FD898* mHighWaterMark;
    /* 0x08 */ TweakValueIntImpl_804FD898* mBudgetTweak;
    /* 0x0C */ char mName[0x20];
    /* 0x2C */ int mBudget;
    /* 0x30 */ u16 mId;
    /* 0x32 */ u16 mFlags;
}; // size: 0x34

class EmissionManager
{
protected:
    EmissionManager();
    ~EmissionManager();

public:
    static EmissionManager* Instance();
    static EmissionManager& InstanceForReplayOnly();

    void Shutdown();
    void Update(float dt);
    int GetNumLights();
    EffectsLight* GetLight(int index);
    void AddEffectsLight(const EffectsLight& light);
    void Render();
    nlDLListContainer<EmissionController*>* GetContainer();
    bool IsStillAlive(EmissionController* controller);
    void Destroy(unsigned long userData, const EffectsGroup* pEffectsGroup);
    void DestroyAll(int view, bool exceptPersistent);
    void DestroyAll(bool exceptPersistent);
    void Kill(unsigned long userData, const EffectsGroup* pEffectsGroup);
    bool IsPlaying(
        unsigned long userData, const EffectsGroup* pEffectsGroup);
    void AddError(const char* format, ...);
    void Replay(LoadFrame& frame);
    void Replay(SaveFrame& frame);
    void KillOldest(int num, bool lingeringOnly);
    void SetContext(void* context);

    /* 0x000 */ EmissionResourceStats mResourceStats[8];
    /* 0x1A0 */ void* mMemoryContext;
    /* 0x1A4 */ u32 mNextControllerId;
    /* 0x1A8 */ bool m_bRecording;
    /* 0x1A9 */ u8 unknown_0x1A9[0x03];
    /* 0x1AC */ void* mContext;
    /* 0x1B0 */ bool unknown_0x1B0;
    /* 0x1B1 */ u8 unknown_0x1B1[0x03];
    /* 0x1B4 */ nlDLListContainer<EmissionController*> mReplayControllers;
    /* 0x1BC */ nlDLListContainer<EmissionController*> mControllers;
    /* 0x1C4 */ nlDLListContainer<EmissionController*> mUnidentifiedControllers;
    /* 0x1CC */ int mNumParticles;
    /* 0x1D0 */ void* mParticleMemory;
    /* 0x1D4 */ nlDLListSlotPool<Particle*> mParticles;
    /* 0x1F0 */ bool mUpdateEnabled;
    /* 0x1F1 */ bool mRenderPersistentOnly;
    /* 0x1F2 */ u8 unknown_0x1F2[0x02];
    /* 0x1F4 */ float mTimeScale;
    /* 0x1F8 */ bool unknown_0x1F8;
    /* 0x1F9 */ u8 unknown_0x1F9[0x03];
};

extern "C" void fn_802E6C20(EmissionManager*, void*, int, int);
EmissionManager* GetEmissionManager();
extern "C" void fn_802E6620(bool, bool, bool, bool);
extern "C" bool fn_802E6774(void*);
extern "C" void fn_802E9E0C(int resource, int budget);
extern "C" void fn_802E9E9C(
    int resource, const char* name, int budget);
extern "C" void fn_802E9F94(
    unsigned long resource, int numParticles);

void fxSetTerrain(unsigned long terrainID);
u32 fxGetTerrain();

#endif // GAME_EFFECTS_EMISSION_MANAGER_H
