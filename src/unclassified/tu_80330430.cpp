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

void RegisterNetworkMessages_80330430()
{
    lbl_806E2100->fn_8032CA40(2, &sFactoryType2);
    lbl_806E2100->fn_8032CA40(3, &sFactoryType3);
    lbl_806E2100->fn_8032CA40(4, &sFactoryType4);
    lbl_806E2100->fn_8032CA40(5, &sFactoryType5);
    lbl_806E2100->fn_8032CA40(6, &sFactoryType6);
    lbl_806E2100->fn_8032CA40(7, &sFactoryType7);
    lbl_806E2100->fn_8032CA40(10, &sFactoryType10);
    lbl_806E2100->fn_8032CA40(11, &sFactoryType11);
    lbl_806E2100->fn_8032CA40(12, &sFactoryType12);
}
