#ifndef UNCLASSIFIED_TU_80252180_H
#define UNCLASSIFIED_TU_80252180_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLInstance;
struct TU80300104Event;

struct TU802384AC
{
    TU802384AC(bool enabled);
    virtual ~TU802384AC();
    virtual void fn_802385D0(TU80300104Event event, float fDeltaT);
    virtual void fn_802385C0(TLComponentInstance* component);
    virtual void fn_802385C8(TLComponentInstance* component);

    /* 0x004 */ bool mUnidentified004;
    /* 0x005 */ bool mUnidentified005[2];
    /* 0x007 */ bool mUnidentified007[2];
    /* 0x009 */ bool mUnidentified009[2];
    /* 0x00B */ bool mUnidentified00B;
    /* 0x00C */ bool mUnidentified00C;
    /* 0x00D */ bool mUnidentified00D;
    /* 0x00E */ bool mUnidentified00E;
    /* 0x00F */ unsigned char mPadding00F;
    /* 0x010 */ int mUnidentified010;
    /* 0x014 */ float mUnidentified014;
    /* 0x018 */ unsigned char mUnidentified018[0x170];
}; // size 0x188

class TU80252180Scene : public BaseSceneHandler
{
public:
    TU80252180Scene();
    virtual ~TU80252180Scene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x01C */ TLComponentInstance* mUnidentified01C[4];
    /* 0x02C */ TLComponentInstance* mUnidentified02C;
    /* 0x030 */ TLComponentInstance* mUnidentified030;
    /* 0x034 */ TLComponentInstance* mUnidentified034;
    /* 0x038 */ TLComponentInstance* mUnidentified038;
    /* 0x03C */ TLComponentInstance* mUnidentified03C;
    /* 0x040 */ TLComponentInstance* mUnidentified040;
    /* 0x044 */ TLComponentInstance* mUnidentified044;
    /* 0x048 */ TLComponentInstance* mUnidentified048;
    /* 0x04C */ TLComponentInstance* mUnidentified04C;
    /* 0x050 */ TLComponentInstance* mUnidentified050;
    /* 0x054 */ TLInstance* mUnidentified054;
    /* 0x058 */ TU802384AC mUnidentified058;
    /* 0x1E0 */ int mUnidentified1E0;
    /* 0x1E4 */ unsigned char mUnidentified1E4;
    /* 0x1E5 */ bool mUnidentified1E5;
    /* 0x1E6 */ bool mUnidentified1E6;
    /* 0x1E7 */ bool mUnidentified1E7;
    /* 0x1E8 */ bool mUnidentified1E8;
    /* 0x1E9 */ unsigned char mPadding1E9[3];
}; // size 0x1EC

extern "C" void fn_80253284(bool value);
extern "C" void fn_80253348(TU80252180Scene* scene, int mask, bool visible);
extern "C" void fn_802533F0(TU80252180Scene* scene);
extern "C" void fn_80253474(TU80252180Scene* scene);
extern "C" void fn_802534BC(TU80252180Scene* scene, int value, bool enabled);
extern "C" TLComponentInstance* fn_80253D70(TU80252180Scene* scene, int value);
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_80253E24(TU80252180Scene* scene);
extern "C" void fn_80253F30(TU80252180Scene* scene);

#endif // UNCLASSIFIED_TU_80252180_H
