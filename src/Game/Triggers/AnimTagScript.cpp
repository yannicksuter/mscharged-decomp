#include "Game/Triggers/AnimTagScript.h"

#include "Game/Inventory.h"
#include "Game/SAnim.h"
#include "Game/Triggers/BinaryTriggerFile.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlMemory.h"

#include <string.h>

u8 AnimTagScriptInterpreter::SetupAnimationTriggers(void* pFileData,
    unsigned long FileSize, cInventory<cSAnim>* pAnimInventory)
{
    BinaryTriggerFile file(pFileData, FileSize);
    u8 result = SetupAnimationTriggers(file, pAnimInventory);
    nlFree(file.m_pFileData);
    return result;
}

u8 AnimTagScriptInterpreter::SetupAnimationTriggers(BinaryTriggerFile& file,
    cInventory<cSAnim>* pAnimInventory)
{
    for (nlListIterator<cSAnim*> iterator = pAnimInventory->Begin();
        iterator.IsValid(); iterator.Next())
    {
        u32 key = iterator.Current()->GetHashID();
        file.m_pCurrentAnim = nlBSearch<BinaryTriggerFile::ANIM_RECORD,
            unsigned long>(
            key, file.m_pFirstAnim, file.m_pFileData->AnimCount);

        if (file.m_pCurrentAnim != NULL)
        {
            AnimTagCBInfo* pSlot;
            const BinaryTriggerFile::TRIGGER_RECORD* pTriggerRecord;
            file.m_CurrentTrigger = 0;
            while (pTriggerRecord = file.m_pFirstTrigger
                    + file.m_pCurrentAnim->TriggerOffset
                    + file.m_CurrentTrigger,
                file.m_CurrentTrigger < file.m_pCurrentAnim->TriggerCount)
            {
                pSlot = NULL;
                m_AnimTagSlotPool.Allocate(pSlot);

                pSlot->pAnimTagScript = this;
                pSlot->ScriptInfo.Trigger = pTriggerRecord->Trigger;
                pSlot->ScriptInfo.ScriptFuncOffset
                    = pTriggerRecord->ScriptFuncOffset;

                float fTime = pTriggerRecord->Frame;
                fTime /= (float)iterator.Current()->m_nNumKeys;
                iterator.Current()->CreateCallback(fTime, (unsigned int)pSlot,
                    AnimTagScriptInterpreter::AnimControllerCB);

                file.m_CurrentTrigger++;
            }
        }
    }

    m_ppBytecode[m_BytecodeCount]
        = nlMalloc(file.m_FileSize
                - ((u32)((u8*)file.m_pFileData
                       + file.m_pFileData->BytecodeOffset)
                    - (u32)file.m_pFileData),
            8, false);
    memcpy(m_ppBytecode[m_BytecodeCount],
        (u8*)file.m_pFileData + file.m_pFileData->BytecodeOffset,
        file.m_FileSize
            - ((u32)((u8*)file.m_pFileData
                   + file.m_pFileData->BytecodeOffset)
                - (u32)file.m_pFileData));
    LoadByteCode(m_ppBytecode[m_BytecodeCount]);
    m_BytecodeCount++;

    return 0;
}
