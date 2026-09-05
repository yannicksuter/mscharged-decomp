#ifndef GAME_AUDIO_AUDIO_SOURCE_8035EF08_H
#define GAME_AUDIO_AUDIO_SOURCE_8035EF08_H

#include "revolution/sp.h"
#include "NL/nlFile.h"
#include "NL/nlSlotPool.h"

class Plat3dSoundSrc;
class AudioReadState_80361258;
struct AudioStreamChannel_8035FA38;

struct AudioSourceData_8035C298
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned char m_Unknown08;
};

struct AudioSourceResource_8035C298
{
    unsigned char m_Unknown00[0x10];
    AudioSourceData_8035C298* m_Unknown10;
    void* m_Unknown14;
    union
    {
        SPSoundTable* m_SoundTable;
        nlFile* m_File;
    } m_Unknown18;
};

struct AudioSourceInfo_8035C298
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned int m_Unknown08;
    unsigned int m_Unknown0C;
    unsigned int m_Unknown10;
    unsigned int m_Unknown14;
    AudioSourceResource_8035C298* m_Unknown18;
};

class AudioSource_8035C234
{
public:
    AudioSource_8035C234()
    {
        m_Unknown04 = 0;
        m_Unknown08 = 0;
        m_Unknown10 = 0;
        m_Unknown14_00 = 0;
        m_Unknown14_0C = 1;
        m_Unknown14_18 = 0;
    }
    virtual ~AudioSource_8035C234() { }
    virtual void fn_0C();
    virtual unsigned int fn_10();
    virtual bool fn_14();
    virtual bool fn_18();
    virtual bool fn_1C() = 0;
    virtual bool fn_20(unsigned int) = 0;
    virtual void fn_24() = 0;
    virtual bool fn_28() = 0;
    virtual bool fn_2C() = 0;
    virtual void fn_30(float) = 0;
    virtual void fn_34(float) = 0;
    virtual void fn_38(float) = 0;
    virtual void fn_3C(Plat3dSoundSrc*);
    virtual void fn_40(AudioSourceInfo_8035C298*) = 0;
    virtual void fn_44() = 0;
    virtual bool fn_48();
    virtual void fn_4C(float) = 0;
    virtual void fn_50(float) = 0;
    virtual void fn_54(int) = 0;
    virtual void fn_58(bool, unsigned int, bool) = 0;
    virtual void fn_5C(int, int) = 0;
    virtual bool fn_60() = 0;
    virtual bool fn_64() = 0;
    virtual AXVPB* fn_68() = 0;
    virtual void fn_6C(bool) = 0;

    void fn_8035F3AC(bool, unsigned int);

    /* 0x04 */ unsigned int m_Unknown04;
    /* 0x08 */ AudioSourceInfo_8035C298* m_Unknown08;
    /* 0x0C */ float m_Unknown0C;
    /* 0x10 */ unsigned int m_Unknown10;
    /* 0x14 */ unsigned int m_Unknown14_00 : 12;
    unsigned int m_Unknown14_0C : 12;
    unsigned int m_Unknown14_18 : 1;
    unsigned int m_Unknown14_19 : 2;
    unsigned int m_Unknown14_1B : 5;
};

class AudioSampleSource_8035F474 : public AudioSource_8035C234
{
public:
    AudioSampleSource_8035F474();
    virtual ~AudioSampleSource_8035F474();
    virtual bool fn_1C();
    virtual bool fn_20(unsigned int);
    virtual void fn_24();
    virtual bool fn_28();
    virtual bool fn_2C();
    virtual void fn_30(float);
    virtual void fn_34(float);
    virtual void fn_38(float);
    virtual void fn_40(AudioSourceInfo_8035C298*);
    virtual void fn_44();
    virtual void fn_4C(float);
    virtual void fn_50(float);
    virtual void fn_54(int);
    virtual void fn_58(bool, unsigned int, bool);
    virtual void fn_5C(int, int);
    virtual bool fn_60();
    virtual bool fn_64();
    virtual AXVPB* fn_68();
    virtual void fn_6C(bool);

    static void operator delete(void* pointer);
    static void fn_8035F96C(void*);

    /* 0x18 */ unsigned int m_Unknown18;
    /* 0x1C */ AXVPB* m_Unknown1C;
    /* 0x20 */ SPSoundEntry* m_Unknown20;
    /* 0x24 */ unsigned int m_Unknown24;
    /* 0x28 */ bool m_Unknown28;
};

struct AudioReadQueueEntry_80361258
{
    AsyncEntry* m_Unknown00;
    AudioReadQueueEntry_80361258* m_next;
};

struct AudioStreamHeader_8035FAE0
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned int m_Unknown08;
    unsigned char m_Unknown0C[0x10];
    AXPBADPCM m_Unknown1C;
};

struct AudioStreamChannel_8035FA38
{
    AudioStreamChannel_8035FA38();
    ~AudioStreamChannel_8035FA38();
    void fn_8035FAE0(AudioStreamHeader_8035FAE0*);
    void fn_8035FD2C(bool);
    static void fn_8035FCC0(void*);

    /* 0x00 */ AudioReadState_80361258* m_Unknown00;
    /* 0x04 */ AXVPB* m_Unknown04;
    /* 0x08 */ void* m_Unknown08;
    /* 0x0C */ unsigned int m_Unknown0C;
    /* 0x10 */ unsigned int m_Unknown10_00 : 31;
    unsigned int m_Unknown10_1F : 1;
    /* 0x14 */ unsigned int m_Unknown14;
};

class AudioReadState_80361258 : public AudioSource_8035C234
{
public:
    AudioReadState_80361258();
    virtual ~AudioReadState_80361258();
    virtual bool fn_1C();
    virtual bool fn_20(unsigned int);
    virtual void fn_24();
    virtual bool fn_28();
    virtual bool fn_2C();
    virtual void fn_30(float);
    virtual void fn_34(float);
    virtual void fn_38(float);
    virtual void fn_40(AudioSourceInfo_8035C298*);
    virtual void fn_44();
    virtual void fn_50(float);
    virtual void fn_58(bool, unsigned int, bool);
    virtual void fn_5C(int, int);
    virtual unsigned int fn_70() = 0;
    virtual int fn_74() = 0;
    virtual AudioStreamChannel_8035FA38* fn_78() = 0;
    virtual AudioStreamChannel_8035FA38* fn_7C() = 0;
    virtual AudioStreamChannel_8035FA38* fn_80(AudioStreamChannel_8035FA38*) = 0;

    /* 0x18 */ unsigned int m_Unknown18;
    /* 0x1C */ unsigned int m_Unknown1C;
    /* 0x20 */ signed int m_Unknown20_00 : 7;
    unsigned int m_Unknown20_07 : 24;
    unsigned int m_Unknown20_1F : 1;
    /* 0x24 */ AudioReadQueueEntry_80361258* m_Unknown24;
    /* 0x28 */ int m_Unknown28;
};

extern "C"
{
    void fn_8035EF08(AXVPB*, float);
    void fn_8035EF3C(AXVPB*, float);
    void fn_8035F06C(AXVPB*, float, float);
    void fn_8035F0D8(AXVPB*, float);
    void fn_8035F120(AXVPB*, float);
    void fn_8035F180(AXVPB*, int);
    void fn_8035F1AC(AXVPB*, bool, unsigned int, bool);
    void fn_8035F24C(AXVPB*, int, int);
    void fn_80361258(AudioReadState_80361258*, AsyncEntry*);
    void fn_80361308(AudioReadState_80361258*);
    void fn_80361778(nlFile*, void*, unsigned int, unsigned long, ReadAsyncCallback);
}

#endif // GAME_AUDIO_AUDIO_SOURCE_8035EF08_H
