#ifndef GAME_EFFECTS_EMISSION_MANAGER_H
#define GAME_EFFECTS_EMISSION_MANAGER_H

#include "NL/nlDLListContainer.h"
#include "NL/nlFunction.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "types.h"

class EmissionController;
class EffectsGroup;
class GLInventory;
class GLResourcePool;
class LoadFrame;
class Particle;
class SaveFrame;
class TweakIntBinding;

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
    void Initialize();

    /* 0x00 */ TweakIntBinding* mCount;
    /* 0x04 */ TweakIntBinding* mHighWaterMark;
    /* 0x08 */ TweakIntBinding* mBudgetTweak;
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

    float GetUnidentified1F4() const
    {
        return mTimeScale;
    }

    EffectsGroup* GetEffectsGroup(const char* name);
    EmissionController* Create(EffectsGroup* pEffectsGroup, int view, bool addToEnd, unsigned short id);

    void Startup(void* context, int numParticles, int maxRenderedParticles);
    void Shutdown();
    void Update(float dt);
    int GetNumLights();
    EffectsLight* GetLight(int index);
    void AddEffectsLight(const EffectsLight& light);
    void Render();
    nlDLListContainer<EmissionController*>* GetContainer();
    bool IsStillAlive(EmissionController* controller);
    void Destroy(unsigned long userData, const EffectsGroup* pEffectsGroup);
    void Destroy(const EffectsGroup* pEffectsGroup);
    void ForEachController(const Function1<void, EmissionController&>& callback);
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
    EmissionController* FindController(unsigned long userData, const EffectsGroup* pEffectsGroup);
    void Kill(const EffectsGroup* pEffectsGroup);
    bool IsDying(unsigned long userData, const EffectsGroup* pEffectsGroup);
    void KillAll();
    void PrepareForReplay();
    static void SetResourceBudget(int resource, int budget);
    static void ConfigureResource(int resource, const char* name, int budget);
    static void RecordRenderedParticles(unsigned long resource, int numParticles);
    static void StartLoading(bool first, bool second, bool third, bool fourth);
    static bool FinishLoading(GLResourcePool* context);
    static void LoadBundle(void* data, void* nonResidentData, GLResourcePool* context, int bundleType);

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

extern "C" EmissionController* fn_802E7DC4(
    EmissionManager*, const char*, int, bool, unsigned short);

extern "C" void Startup(EmissionManager*, void*, int, int);
EmissionManager* GetEmissionManager();

extern GLInventory* gEffectsModelInventory;

void fxSetTerrain(unsigned long terrainID);
u32 fxGetTerrain();

#endif // GAME_EFFECTS_EMISSION_MANAGER_H
