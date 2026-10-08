#include "Game/Triggers/BinaryTriggerFile.h"

BinaryTriggerFile::BinaryTriggerFile(
    void* pFileData, unsigned long FileSize)
{
    m_FileSize = FileSize;
    m_pFileData = (BinaryTriggerFileHeader*)pFileData;
    m_pCurrentAnim = 0;
    m_CurrentTrigger = 0;
    m_pFirstAnim
        = (ANIM_RECORD*)((u8*)m_pFileData + sizeof(BinaryTriggerFileHeader));
    m_pFirstTrigger = (TRIGGER_RECORD*)((u8*)m_pFirstAnim
        + (m_pFileData->AnimCount * sizeof(ANIM_RECORD)));
}
