#ifndef UNCLASSIFIED_TU_8026F444_H
#define UNCLASSIFIED_TU_8026F444_H

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8030616C.h"

class NetMessageCheckConnection;
class NetworkMessageType27_8050B750;
class TLComponentInstance;

class TU8026F444Scene : public BaseSceneHandler
{
public:
    TU8026F444Scene();
    virtual ~TU8026F444Scene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void fn_8026F7F8(NetMessageCheckConnection* message);
    void fn_8026F80C(UnidentifiedTimer_8030616C* timer);
    void fn_8026F874(UnidentifiedTimer_8030616C* timer);
    void fn_8026F878();
    void fn_8026FBB0(int index, void* context);
    void fn_8026FF28(NetworkMessageType27_8050B750* message);
    void fn_80270870();
    void fn_80271640(int index, void* context);
    void fn_802716E0(int index, void* context);
    void fn_80271768();

    /* 0x01C */ unsigned int mUnidentified01C;
    /* 0x020 */ int mUnidentified020[4];
    /* 0x030 */ bool mUnidentified030;
    /* 0x031 */ bool mUnidentified031;
    /* 0x032 */ unsigned char mPadding032[2];
    /* 0x034 */ int mUnidentified034;
    /* 0x038 */ int mUnidentified038;
    /* 0x03C */ int mUnidentified03C[2];
    /* 0x044 */ unsigned short mUnidentified044[128];
    /* 0x144 */ UnidentifiedTimer_8030616C mUnidentified144;
    /* 0x160 */ UnidentifiedTimer_8030616C mUnidentified160;
    /* 0x17C */ bool mUnidentified17C;
    /* 0x17D */ unsigned char mPadding17D[3];
    /* 0x180 */ int mUnidentified180;
    /* 0x184 */ unsigned int mUnidentified184[2];
    /* 0x18C */ TU80219248Component mUnidentified18C[2];
    /* 0x2F4 */ TLComponentInstance* mUnidentified2F4[2];
    /* 0x2FC */ TLComponentInstance* mUnidentified2FC;
    /* 0x300 */ bool mUnidentified300;
}; // size 0x304

#endif // UNCLASSIFIED_TU_8026F444_H
