#include "unclassified/tu_80330430.h"

static NetworkMessageFactory<NetworkMessageType2_805333DC> sFactoryType2;
static NetworkMessageFactory<NetworkMessageType3_805333C8> sFactoryType3;
static NetworkMessageFactory<NetworkMessageType4_80533468> sFactoryType4;
static NetworkMessageFactory<NetworkMessageType5_80533454> sFactoryType5;
static NetworkMessageFactory<NetworkMessageType6_8053342C> sFactoryType6;
static NetworkMessageFactory<NetworkMessageType7_80533440> sFactoryType7;
static NetworkMessageFactory<NetworkMessageType10_80533418> sFactoryType10;
static NetworkMessageFactory<NetworkMessageType11_80533404> sFactoryType11;
static NetworkMessageFactory<NetworkMessageType12_805333F0> sFactoryType12;

extern void* lbl_806E2100;
extern "C" void fn_8032CA40(
    void* registry, int type, UnidentifiedNetworkMessageFactory* factory);

void RegisterNetworkMessages_80330430()
{
    fn_8032CA40(lbl_806E2100, 2, &sFactoryType2);
    fn_8032CA40(lbl_806E2100, 3, &sFactoryType3);
    fn_8032CA40(lbl_806E2100, 4, &sFactoryType4);
    fn_8032CA40(lbl_806E2100, 5, &sFactoryType5);
    fn_8032CA40(lbl_806E2100, 6, &sFactoryType6);
    fn_8032CA40(lbl_806E2100, 7, &sFactoryType7);
    fn_8032CA40(lbl_806E2100, 10, &sFactoryType10);
    fn_8032CA40(lbl_806E2100, 11, &sFactoryType11);
    fn_8032CA40(lbl_806E2100, 12, &sFactoryType12);
}
