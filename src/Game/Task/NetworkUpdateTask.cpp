#include "Game/Task/NetworkUpdateTask.h"

#include "types.h"

struct NetworkManager
{
    u8 padding[4];
    u8 enabled;
};

struct NetworkStatus
{
    u8 padding[0xD];
    u8 enabled;
};

struct NetworkSubsystem
{
    u8 padding[0x242C];
    virtual void Unknown0();
    virtual void Unknown1();
    virtual void Unknown2();
    virtual void Unknown3();
    virtual void Unknown4();
    virtual void Unknown5();
    virtual void Unknown6();
    virtual void Unknown7();
    virtual void Unknown8();
    virtual void Update();
};

extern NetworkManager* lbl_806E2138;
extern NetworkStatus* lbl_806E2168;
extern NetworkSubsystem* lbl_806E20D8;
extern void* lbl_806E1194;
extern s32 lbl_806E1188;
extern u8 lbl_806E1008;

extern "C" void fn_803740B8();
extern "C" void fn_8011E384();
extern "C" void fn_80338898();
extern "C" void fn_80337F68();
extern "C" void fn_8032C7D0();
extern "C" void fn_801258A8();
extern "C" void fn_803327DC();
extern "C" void* fn_8011166C();
extern "C" void fn_803328AC(NetworkManager*, void*);
extern "C" void fn_80332EDC();
extern "C" void fn_801274A4();
extern "C" void fn_8012AE4C();
extern "C" void* fn_802AA79C(u32, u32, u32);
extern "C" void* fn_801360A4(void*);
extern "C" void fn_8012F378();
extern "C" s32 fn_8011C19C();

void NetworkUpdateTask::Initialize()
{
    fn_803740B8();
    fn_8011E384();
    fn_80338898();
    fn_80337F68();
    fn_8032C7D0();
    fn_801258A8();
    fn_803327DC();

    void* handler = fn_8011166C();
    if (handler != 0)
    {
        handler = static_cast<u8*>(handler) + 0x20;
    }
    fn_803328AC(lbl_806E2138, handler);

    fn_80332EDC();
    fn_801274A4();
    fn_8012AE4C();

    if (lbl_806E1194 == 0)
    {
        void* instance = fn_802AA79C(0xEB0, 8, 0);
        if (instance != 0)
        {
            instance = fn_801360A4(instance);
        }
        lbl_806E1194 = instance;
    }

    fn_8012F378();
    if (fn_8011C19C() == 2)
    {
        lbl_806E1188 = 9;
    }
    else if (fn_8011C19C() == 0)
    {
        lbl_806E1188 = -8;
    }

    lbl_806E1008 = 0;
    lbl_806E2138->enabled = 1;
    lbl_806E2168->enabled = 0;
}

void NetworkUpdateTask::Run(float)
{
    lbl_806E20D8->Update();
}
