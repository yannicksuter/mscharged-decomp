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
