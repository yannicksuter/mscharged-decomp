#ifndef _SHCROSSFADER_H_
#define _SHCROSSFADER_H_

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLInstance;

class CrossFaderScene : public BaseSceneHandler
{
public:
    CrossFaderScene();
    virtual ~CrossFaderScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ int mNumImages;
    /* 0x20 */ int mCurrentImage;
    /* 0x24 */ TLInstance** mImageInstances;
    /* 0x28 */ TLInstance* mCurrentImageInstance;
    /* 0x2C */ TLComponentInstance* mHomeMessage;
    /* 0x30 */ bool mWidescreen;
    /* 0x31 */ u8 mPadding31[3];
    /* 0x34 */ float mTimer;
    /* 0x38 */ float mAlpha;
    /* 0x3C */ int mFadeState;
    /* 0x40 */ float mFadeToBlackTimer;
}; // size 0x44

#endif // _SHCROSSFADER_H_
