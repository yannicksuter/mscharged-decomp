#ifndef UNCLASSIFIED_TU_8022F710_H
#define UNCLASSIFIED_TU_8022F710_H

#include "Game/FE/feLibObject.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;
class TLInstance;

class UnidentifiedScrollWidget
{
public:
    UnidentifiedScrollWidget();
    ~UnidentifiedScrollWidget();

    void fn_8022F848(bool enabled);
    void fn_8022F858();
    bool fn_8022FD80(int direction, int value);
    void fn_8022FDDC(int index, void* context);
    void fn_8022FE94(int index, void* context);
    void fn_8022FFA8(int index, void* context);
    void fn_802300A4(int index, void* context);
    void fn_802301B8(int index, void* context);
    void fn_8023038C(const nlVector3& value);
    void fn_802303CC();
    void fn_80230468(TU80300104Event event, float dt);
    void fn_802308D0(TLInstance* instance);
    void fn_80230B90(int value);
    void fn_80230DE0(int value);
    void fn_80230EE0(int index, void* context);

    /* 0x000 */ TLComponentInstance* mUnidentified00;
    /* 0x004 */ TLComponentInstance* mUnidentified04[2];
    /* 0x00C */ TLInstance* mUnidentified0C;
    /* 0x010 */ bool mUnidentified10[2];
    /* 0x012 */ u8 mUnidentified12[2];
    /* 0x014 */ bool mUnidentified14[2];
    /* 0x016 */ bool mUnidentified16[2];
    /* 0x018 */ bool mUnidentified18;
    /* 0x019 */ bool mUnidentified19;
    /* 0x01A */ bool mUnidentified1A;
    /* 0x01B */ bool mUnidentified1B;
    /* 0x01C */ feVector3 mUnidentified1C;
    /* 0x028 */ float mUnidentified28;
    /* 0x02C */ float mUnidentified2C;
    /* 0x030 */ float mUnidentified30;
    /* 0x034 */ int mUnidentified34;
    /* 0x038 */ int mUnidentified38;
    /* 0x03C */ nlVector3 mUnidentified3C;
    /* 0x048 */ float mUnidentified48;
    /* 0x04C */ TU80219248Component mComponents[2];
}; // size 0x1B4

#endif // UNCLASSIFIED_TU_8022F710_H
