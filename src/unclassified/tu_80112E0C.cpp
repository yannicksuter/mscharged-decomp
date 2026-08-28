#include "NL/nlTask.h"

#include "types.h"

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
    virtual const char* GetName();
    virtual void Run(float dt);

    static bool s_FramerateLocked;
    static eModelSkinMethod s_GameplaySkin;
    static eModelSkinMethod s_ReplaySkin;
};

extern const f32 lbl_806E45F8;
extern const f32 lbl_806E45FC;

extern "C" float fn_80112E0C()
{
    return lbl_806E45F8;
}

extern "C" float fn_80112E14()
{
    return lbl_806E45FC;
}

extern "C" bool fn_80112E1C()
{
    return false;
}
