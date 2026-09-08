#ifndef UNCLASSIFIED_TU_8025D170_H
#define UNCLASSIFIED_TU_8025D170_H

#include "Game/BaseSceneHandler.h"
#include "Game/NetworkStatsManager.h"
#include "unclassified/tu_8022F710.h"
#include "NL/nlString.h"
#include "unclassified/tu_8022EF84.h"

struct UnidentifiedDraftEntry;

struct TU8025D170Row
{
    TU8025D170Row();
    void fn_8025E438();

    /* 0x00 */ u16 mUnidentified00[14];
    /* 0x1C */ u8 mUnidentified1C[0x4C];
    /* 0x68 */ int mUnidentified68;
    /* 0x6C */ int mUnidentified6C;
    /* 0x70 */ int mUnidentified70;
    /* 0x74 */ NetworkRankingMeta mUnidentified74;
    /* 0x8C */ int mUnidentified8C;
    /* 0x90 */ int mUnidentified90;
    /* 0x94 */ bool mUnidentified94;
    /* 0x95 */ bool mUnidentified95;
    /* 0x96 */ bool mUnidentified96;
}; // size 0x98

struct TU8025D170Slot
{
    /* 0x00 */ bool mUnidentified00;
    /* 0x01 */ bool mUnidentified01;
    /* 0x04 */ UnidentifiedDraftEntry* mUnidentified04;
}; // size 0x08

class SHOnlineInvitePlayers : public BaseSceneHandler
{
public:
    SHOnlineInvitePlayers();
    virtual ~SHOnlineInvitePlayers();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_8025D3C8();
    void fn_8025D520();
    void fn_8025DE24(int index, void* context);
    void fn_8025DEBC(int index, void* context);
    void fn_8025DF3C(int index, void* context);
    void fn_8025DFAC(int index, void* context);
    void fn_8025E044(int index, void* context);
    void fn_8025E0C4(int index, void* context);
    void fn_8025E128(int index, void* context);
    void fn_8025E19C(int index, void* context);
    void fn_8025E1F8(int index, void* context);
    void fn_8025E394();
    bool fn_8025ED98();
    void fn_8025F8A4(int value, int index);
    void fn_8025FF58(int index);
    void fn_8026066C();

    /* 0x01C */ bool mUnidentified01C;
    /* 0x01D */ bool mUnidentified01D;
    /* 0x01E */ bool mUnidentified1E;
    /* 0x020 */ int mUnidentified20[4];
    /* 0x030 */ bool mUnidentified30;
    /* 0x034 */ float mUnidentified34;
    /* 0x038 */ int mUnidentified38;
    /* 0x03C */ float mUnidentified3C;
    /* 0x040 */ bool mUnidentified40;
    /* 0x041 */ bool mUnidentified41;
    /* 0x042 */ bool mUnidentified42;
    /* 0x044 */ TU8022EF84Component mUnidentified44;
    /* 0x11C */ UnidentifiedScrollWidget mUnidentified11C;
    /* 0x2D0 */ TLComponentInstance* mUnidentified2D0;
    /* 0x2D4 */ TU80219248Component mUnidentified2D4;
    /* 0x388 */ TLComponentInstance* mUnidentified388;
    /* 0x38C */ TLComponentInstance* mUnidentified38C[4];
    /* 0x39C */ TU80219248Component mUnidentified39C[4];
    /* 0x66C */ TLComponentInstance* mUnidentified66C[4];
    /* 0x67C */ TU80219248Component mUnidentified67C[4];
    /* 0x94C */ int mUnidentified94C;
    /* 0x950 */ TU8025D170Slot mUnidentified950[4];
    /* 0x970 */ int mUnidentified970;
    /* 0x974 */ TU8025D170Row mUnidentified974[4];
    /* 0xBD4 */ u16 mUnidentifiedBD4[4][0x20];
    /* 0xCD4 */ u16 mUnidentifiedCD4[4][0x30];
    /* 0xE54 */ u8 mUnidentifiedE54[8];
}; // size 0xE5C

inline TU8025D170Row::TU8025D170Row()
{
    fn_8025E438();
}

inline void TU8025D170Row::fn_8025E438()
{
    mUnidentified6C = 4;
    mUnidentified70 = 11;
    mUnidentified8C = -1;
    mUnidentified94 = false;
    mUnidentified90 = 0;
    mUnidentified95 = false;
    mUnidentified96 = false;
    mUnidentified00[0] = 0;
    memset(mUnidentified1C, 0, sizeof(mUnidentified1C));
    mUnidentified68 = -1;
    mUnidentified74.mScore = 0;
    mUnidentified74.mDisplayRank = 0;
    mUnidentified74.mWins = 0;
    mUnidentified74.mLosses = 0;
    mUnidentified74.mUnidentified14 = 0;
}


#endif // UNCLASSIFIED_TU_8025D170_H
