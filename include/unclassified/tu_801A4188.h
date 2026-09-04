#ifndef UNCLASSIFIED_TU_801A4188_H
#define UNCLASSIFIED_TU_801A4188_H

#include "Game/Render/ImpostorCharacter.h"
#include "NL/nlMemory.h"

struct CrowdCharacterDefinition_801A4188
{
    const char* mUnidentified00;
    const char* mUnidentified04;
    const char* mUnidentified08;
    const char* mUnidentified0C;
    const char* mUnidentified10;
    const char* mUnidentified14;
}; // size: 0x18

struct CrowdModelCollection_801A4188
{
    CrowdModelCollection_801A4188();
    ~CrowdModelCollection_801A4188();

    /* 0x00 */ bool mUnidentified000;
    /* 0x01 */ bool mUnidentified001;
    /* 0x02 */ bool mUnidentified002;
    /* 0x03 */ bool mUnidentified003;
    /* 0x04 */ cInventory<cSAnim>* mUnidentified004;
    /* 0x08 */ cInventory<cSHierarchy>* mUnidentified008;
    /* 0x0C */ int mUnidentified00C;
    /* 0x10 */ CrowdCharacterDefinition_801A4188* mUnidentified010;
    /* 0x14 */ int mUnidentified014;
    /* 0x18 */ void* mUnidentified018;
    /* 0x1C */ unsigned long mUnidentified01C;
    /* 0x20 */ void* mUnidentified020;
    /* 0x24 */ u32 mUnidentified024;
    /* 0x28 */ void* mUnidentified028;
    /* 0x2C */ unsigned long mUnidentified02C;
    /* 0x30 */ void* mUnidentified030;
    /* 0x34 */ unsigned long mUnidentified034;
    /* 0x38 */ void* mUnidentified038;
    /* 0x3C */ unsigned long mUnidentified03C;
    /* 0x40 */ unsigned long mUnidentified040;
    /* 0x44 */ u32 mUnidentified044;
    /* 0x48 */ ImpostorModel_802DAEE0** models;
    /* 0x4C */ cInventory<cSAnim>** mUnidentified04C;
}; // size: 0x50

extern "C" void fn_802DB9C4(CrowdModelCollection_801A4188* collection);
extern "C" void fn_802DBDA0(CrowdModelCollection_801A4188* collection,
    CrowdCharacterDefinition_801A4188* definitions, int count);
extern "C" bool fn_802DBF5C(CrowdModelCollection_801A4188* collection);
extern "C" void fn_802DBF7C(CrowdModelCollection_801A4188* collection);
extern "C" bool fn_802DC2A4(CrowdModelCollection_801A4188* collection);
extern "C" void fn_802DC6F8(CrowdModelCollection_801A4188* collection);

class ArrayOwner_801A4EC0
{
public:
    ArrayOwner_801A4EC0(int count)
    {
        data = new (8, false) ImpostorModel_802DAEE0*[count];
        capacity = count;
    }
    ~ArrayOwner_801A4EC0();

    ImpostorModel_802DAEE0** data;
    int capacity;
};

class ArrayOwner_801A4F18
{
public:
    ArrayOwner_801A4F18(int count)
    {
        data = new (8, false) ImpostorCharacterImpl_8052E9B8*[count];
        capacity = count;
    }
    ~ArrayOwner_801A4F18();

    ImpostorCharacterImpl_8052E9B8** data;
    int capacity;
};

class ArrayOwner_801A4F70
{
public:
    ArrayOwner_801A4F70(int count)
    {
        data = new (8, false) CrowdCharacterDefinition_801A4188[count];
        capacity = count;
    }
    ~ArrayOwner_801A4F70();

    CrowdCharacterDefinition_801A4188* data;
    int capacity;
};

#endif // UNCLASSIFIED_TU_801A4188_H
