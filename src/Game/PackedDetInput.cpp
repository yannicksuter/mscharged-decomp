#include "Game/PackedDetInput.h"

#include "Game/DetInput.h"

void PackDetInput(PackedDetInput* record, const DetInput* input)
{
    record->mButtonBitfield = input->m_ButtonBitfield;
    record->mTriggers = (input->m_LeftTrigger & 0xF0) | ((input->m_RightTrigger & 0xF0) >> 4);
    record->mRemoteAccel[0] = (s8)(input->m_v3RevRemoteAccel.x * 50.849613f);
    record->mRemoteAccel[1] = (s8)(input->m_v3RevRemoteAccel.y * 50.849613f);
    record->mRemoteAccel[2] = (s8)(input->m_v3RevRemoteAccel.z * 50.849613f);
    record->mFreeStyleAccel[0] = (s8)(input->m_v3RevFreeStyleAccel.x * 50.849613f);
    record->mFreeStyleAccel[1] = (s8)(input->m_v3RevFreeStyleAccel.y * 50.849613f);
    record->mFreeStyleAccel[2] = (s8)(input->m_v3RevFreeStyleAccel.z * 50.849613f);
    record->mDPDNumTargets = input->m_nRevDPDNumTargets;
    record->mDPDCoord[0] = (s8)(input->m_v2RevDPDCoord.x * 127.0f);
    record->mDPDCoord[1] = (s8)(input->m_v2RevDPDCoord.y * 127.0f);
    record->mAnalogAxes[0] = (s8)(input->m_AnalogLeftX * 127.0f);
    record->mAnalogAxes[1] = (s8)(input->m_AnalogLeftY * 127.0f);
    record->mAnalogAxes[2] = (s8)(input->m_AnalogRightX * 127.0f);
    record->mAnalogAxes[3] = (s8)(input->m_AnalogRightY * 127.0f);
}

void UnpackDetInput(const PackedDetInput* record, DetInput* input)
{
    input->m_ButtonBitfield = record->mButtonBitfield;
    input->m_LeftTrigger = record->mTriggers & 0xF0;
    input->m_RightTrigger = (record->mTriggers & 0x0F) << 4;
    input->m_v3RevRemoteAccel.x = record->mRemoteAccel[0] * 0.019665834f;
    input->m_v3RevRemoteAccel.y = record->mRemoteAccel[1] * 0.019665834f;
    input->m_v3RevRemoteAccel.z = record->mRemoteAccel[2] * 0.019665834f;
    input->m_v3RevFreeStyleAccel.x = record->mFreeStyleAccel[0] * 0.019665834f;
    input->m_v3RevFreeStyleAccel.y = record->mFreeStyleAccel[1] * 0.019665834f;
    input->m_v3RevFreeStyleAccel.z = record->mFreeStyleAccel[2] * 0.019665834f;
    input->m_nRevDPDNumTargets = record->mDPDNumTargets;
    input->m_v2RevDPDCoord.x = record->mDPDCoord[0] * 0.007874016f;
    input->m_v2RevDPDCoord.y = record->mDPDCoord[1] * 0.007874016f;
    input->m_AnalogLeftX = record->mAnalogAxes[0] / 127.0f;
    input->m_AnalogLeftY = record->mAnalogAxes[1] / 127.0f;
    input->m_AnalogRightX = record->mAnalogAxes[2] / 127.0f;
    input->m_AnalogRightY = record->mAnalogAxes[3] / 127.0f;
}
