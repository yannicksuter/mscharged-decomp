#ifndef GAME_PARTICLE_UPDATE_TASK_H
#define GAME_PARTICLE_UPDATE_TASK_H

#include "NL/nlFunction.h"
#include "NL/nlTask.h"

class ParticleUpdateTask : public nlTask
{
public:
    ParticleUpdateTask();

    void SetTimeScale(float timeScale);
    virtual void Run(float dt);
    void Shutdown();
    void Initialize(void* context, int parameter1, int parameter2);
    void StartLoading(bool first, bool second, bool third, bool fourth);
    bool FinishLoading(void* context);
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
    int mParameter1;
    int mParameter2;
    bool mRenderEnabled;
    bool mUpdateEnabled;
    unsigned char mCallbackPadding[2];

public:
    Function0<bool> mCanRender;
    Function0<bool> mCanUpdate;
    Function0<void> mBeforeUpdate;
    Function0<void> mUnknownCallback;
};

#endif // GAME_PARTICLE_UPDATE_TASK_H
