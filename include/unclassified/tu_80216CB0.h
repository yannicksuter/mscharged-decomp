#ifndef UNCLASSIFIED_TU_80216CB0_H
#define UNCLASSIFIED_TU_80216CB0_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;

class TU80216CB0Scene : public BaseSceneHandler
{
public:
    virtual ~TU80216CB0Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void fn_8021792C() = 0;
    virtual void fn_80217880(const TU80300104Event* event, int index, float fDeltaT) = 0;

    /* 0x01C */ TU8022EF84Component mUnidentified1C;
    /* 0x0F4 */ void* mUnidentifiedF4;
    /* 0x0F8 */ int mMode;
    /* 0x0FC */ int mControllerCounts[4];
    /* 0x10C */ bool mUnidentified10C;
    /* 0x110 */ float mTrophyTimers[7];
}; // size 0x12C

class TU80217320Scene : public TU80216CB0Scene
{
public:
    TU80217320Scene(int mode);
    virtual ~TU80217320Scene();
    virtual void SceneCreated();
    virtual void fn_8021792C();
    virtual void fn_80217880(const TU80300104Event* event, int index, float fDeltaT);

    void fn_80218104(int index, void* context);
    void fn_80218108(int index, void* context);
    void fn_802182B4(int index, void* context);
    void fn_80218334(int index, void* context);
    void fn_80218548(int index, void* context);
    void fn_802185E4(int index, void* context);
    void fn_80218668(int index, void* context);
    void fn_80218700(unsigned int index);

    /* 0x12C */ TU80219248Component mUnidentified12C;
    /* 0x1E0 */ TU80219248Component mUnidentified1E0[4];
    /* 0x4B0 */ TU80219248Component mUnidentified4B0[2];
    /* 0x618 */ TU80219248Component mUnidentified618;
    /* 0x6CC */ TLComponentInstance* mUnidentified6CC;
    /* 0x6D0 */ unsigned char mUnidentified6D0[0x1C];
    /* 0x6EC */ TLComponentInstance* mUnidentified6EC;
}; // size 0x6F0

class TU80218858Scene : public TU80216CB0Scene
{
public:
    TU80218858Scene();
    virtual ~TU80218858Scene();
    virtual void SceneCreated();
    virtual void fn_8021792C();
    virtual void fn_80217880(const TU80300104Event* event, int index, float fDeltaT);

    void fn_80218D58(int index, void* context);
    void fn_80218E08(int index, void* context);
    void fn_80218EB8(int index, void* context);

    /* 0x12C */ TU80219248Component mUnidentified12C;
    /* 0x1E0 */ TLComponentInstance* mUnidentified1E0;
}; // size 0x1E4

void fn_802172C4();

#endif // UNCLASSIFIED_TU_80216CB0_H
