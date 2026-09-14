#include "Game/Physics/CharacterPhysicsElement.h"

#include "NL/nlChunk.h"
#include "NL/nlMemory.h"

bool LoadCharacterPhysicsElements(void* pFileData, unsigned long nFileSize,
    CharacterPhysicsData* pPhysicsData, bool arg3)
{
    nlChunk* outerChunk;
    nlChunk* endChunk;
    unsigned long i;

    if (pFileData == 0)
    {
        return false;
    }

    endChunk = ((nlChunk*)pFileData)->GetLastChunk();
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
            CharacterPhysicsElement* pPhysicsElements = (CharacterPhysicsElement*)outerChunk->GetData();
            for (i = 0; i < pPhysicsData->physicsElementCount; i++)
            {
                pPhysicsData->pPhysicsElements[i] = pPhysicsElements[i];
            }
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
