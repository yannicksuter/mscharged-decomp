#ifndef GAME_BEGIN_FRAME_TASK_H
#define GAME_BEGIN_FRAME_TASK_H

#include "NL/nlTask.h"

class cBaseCamera;
class nlMatrix4;
struct glModel;

void SetupMatrices(cBaseCamera*, const nlMatrix4*);

extern bool g_bFrameSmiler;
extern bool g_bFrameStatsOnScreen;
extern float lbl_806DC7C8;

enum eModelSkinMethod
{
    eModelSkin_Rigid = 0,
    eModelSkin_Blend = 1,
    eModelSkin_Both = 2,
    eModelSkin_Num = 3,
};

class BeginFrameTask : public nlTask
{
public:
    virtual const char* GetName()
    {
        return "Begin Frame";
    }
    virtual void Run(float dt);

    static eModelSkinMethod s_GameplaySkin;
    static eModelSkinMethod s_ReplaySkin;
    static bool s_FramerateLocked;
};


float fn_80112E0C();
float fn_80112E14();
glModel* fn_80112E1C(glModel* model);

#endif // GAME_BEGIN_FRAME_TASK_H
