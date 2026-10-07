#ifndef GAME_PARTICLE_UPDATE_TASK_H
#define GAME_PARTICLE_UPDATE_TASK_H

#include "NL/nlFunction.h"
#include "NL/nlTask.h"

class GLResourcePool;

class ParticleUpdateTask : public nlTask
{
public:
    ParticleUpdateTask();

    void SetTimeScale(float timeScale);
    virtual void Run(float dt);
    void Shutdown();
    void Initialize(void* context, int numParticles, int maxRenderedParticles);
    void StartLoading(bool allocateAtStart, bool allocateNonResidentAtStart, bool third, bool compressedNonResident);
    bool FinishLoading(GLResourcePool* context);
    virtual const char* GetName()
    {
        return "Particle Update";
    }

    static ParticleUpdateTask* sInstance;

private:
    float mTimeScale;
    bool mResetPending;
    bool mUnknownFlag;
    unsigned char mUnknown[0x82];
    void* mContext;
    int mNumParticles;
    int mMaxRenderedParticles;
    bool mRenderEnabled;
    bool mUpdateEnabled;
    unsigned char mCallbackPadding[2];

public:
    Function<bool()> mCanRender;
    Function<bool()> mCanUpdate;
    Function<FnVoidVoid> mBeforeUpdate;
    Function<FnVoidVoid> mUnknownCallback;
};

#endif // GAME_PARTICLE_UPDATE_TASK_H
