#ifndef UNCLASSIFIED_TU_80254084_H
#define UNCLASSIFIED_TU_80254084_H

#include "Game/BaseSceneHandler.h"
#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;
class TLSlide;
class TU80252180Scene;
struct InlineHasher;
struct TU802384AC;

class TU802554B4Scene : public BaseSceneHandler
{
public:
    TU802554B4Scene(int mode);
    virtual ~TU802554B4Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x01C */ int mMode;
    /* 0x020 */ TU8022EF84Component mNavigation;
    /* 0x0F8 */ TU802384AC* mUnidentified0F8;
    /* 0x0FC */ UnidentifiedScrollWidget mScrollWidget;
    /* 0x2B0 */ unsigned short mUnidentified2B0[0x40];
    /* 0x330 */ unsigned short mUnidentified330[7][0x20];
    /* 0x4F0 */ unsigned char mUnidentified4F0[0x1C0];
    /* 0x6B0 */ int mUnidentified6B0[4];
    /* 0x6C0 */ int mUnidentified6C0;
    /* 0x6C4 */ int mUnidentified6C4;
    /* 0x6C8 */ bool mUnidentified6C8;
    /* 0x6C9 */ bool mUnidentified6C9;
    /* 0x6CA */ bool mUnidentified6CA;
    /* 0x6CB */ unsigned char mPadding6CB;
    /* 0x6CC */ int mUnidentified6CC;
}; // size 0x6D0

class TU80257150Scene : public BaseSceneHandler
{
public:
    TU80257150Scene(int mode);
    virtual ~TU80257150Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_80257850();
    void fn_80257B9C(int index, void* context);
    void fn_80257C60(int index, void* context);
    void fn_80257EA4(int index, void* context);

    /* 0x01C */ TU8022EF84Component mNavigation;
    /* 0x0F4 */ TU80219248Component mComponents[2];
    /* 0x25C */ TLComponentInstance* mButtonInstances[2];
    /* 0x264 */ int mMode;
    /* 0x268 */ int mUnidentified268[4];
    /* 0x278 */ bool mUnidentified278;
    /* 0x279 */ unsigned char mPadding279[3];
    /* 0x27C */ int mComponentCount;
    /* 0x280 */ int mUnidentified280;
    /* 0x284 */ int mUnidentified284;
}; // size 0x288

extern "C" void fn_80254084(int index, TLComponentInstance* component);
extern "C" TLComponentInstance* fn_80254098(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6);
extern "C" TLComponentInstance* fn_80254170(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6);
extern "C" TLSlide* fn_80254238(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6);
extern "C" void fn_80254310(TU80252180Scene* scene, bool enabled);
extern "C" void fn_802547E8(TU80252180Scene* scene, int value);
extern "C" void fn_80254A14(TU80252180Scene* scene, int value);
extern "C" void fn_80254BD4(TU80252180Scene* scene, int value);
extern "C" void fn_80254E3C(TU80252180Scene* scene);
extern "C" void fn_80257D0C(bool value0, unsigned char value1, bool value2);

#endif // UNCLASSIFIED_TU_80254084_H
