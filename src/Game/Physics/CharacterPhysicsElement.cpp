#include "Game/Physics/CharacterPhysicsElement.h"

#include "Game/SAnim.h"
#include "NL/nlMemory.h"

static inline void CopyPhysicsElements(CharacterPhysicsData* pPhysicsData,
    CharacterPhysicsElement* pSrc)
{
    u32 n;
    for (n = 0; n < pPhysicsData->physicsElementCount; n++)
    {
        pPhysicsData->pPhysicsElements[n] = pSrc[n];
    }
}

bool LoadCharacterPhysicsElements(void* pFileData, unsigned long nFileSize,
    CharacterPhysicsData* pPhysicsData, bool arg3)
{
    nlChunk* outerChunk;
    nlChunk* endChunk;

    if (pFileData == 0)
    {
        return false;
    }

    endChunk = ((nlChunk*)pFileData)->GetNextChunk();
    outerChunk = ((nlChunk*)pFileData)->GetFirstChunk();

    while (outerChunk < endChunk)
    {
        switch ((s32)outerChunk->GetID())
        {
        case 0x0001D001:
        {
            pPhysicsData->physicsElementCount = *(u32*)outerChunk->GetData();
            pPhysicsData->pPhysicsElements = (CharacterPhysicsElement*)nlMalloc(
                pPhysicsData->physicsElementCount * sizeof(CharacterPhysicsElement), 8, false);
            break;
        }
        case 0x0001D002:
        {
            CopyPhysicsElements(pPhysicsData, (CharacterPhysicsElement*)outerChunk->GetData());
            break;
        }
        }

        outerChunk = outerChunk->GetNextChunk();
    }

    if (arg3)
    {
        delete (u8*)pFileData;
    }
    return true;
}
