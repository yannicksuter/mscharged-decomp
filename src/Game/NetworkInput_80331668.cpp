#include "Game/NetworkInput_80331668.h"

#include "Game/DetInput.h"

extern "C" void fn_80331668(InputRouterRecord* record, const DetInput* input)
{
    record->mUnidentified00 = input->m_ButtonBitfield;
    record->mUnidentified02 = (input->m_LeftTrigger & 0xF0) | ((input->m_RightTrigger & 0xF0) >> 4);
    record->mUnidentified03[0] = (s8)(input->m_v3RevRemoteAccel.x * 50.849613f);
    record->mUnidentified03[1] = (s8)(input->m_v3RevRemoteAccel.y * 50.849613f);
    record->mUnidentified03[2] = (s8)(input->m_v3RevRemoteAccel.z * 50.849613f);
    record->mUnidentified06[0] = (s8)(input->m_v3RevFreeStyleAccel.x * 50.849613f);
    record->mUnidentified06[1] = (s8)(input->m_v3RevFreeStyleAccel.y * 50.849613f);
    record->mUnidentified06[2] = (s8)(input->m_v3RevFreeStyleAccel.z * 50.849613f);
    record->mUnidentified09 = input->m_nRevDPDNumTargets;
    record->mUnidentified0A[0] = (s8)(input->m_v2RevDPDCoord.x * 127.0f);
    record->mUnidentified0A[1] = (s8)(input->m_v2RevDPDCoord.y * 127.0f);
    record->mUnidentified0C[0] = (s8)(input->m_AnalogLeftX * 127.0f);
    record->mUnidentified0C[1] = (s8)(input->m_AnalogLeftY * 127.0f);
    record->mUnidentified0C[2] = (s8)(input->m_AnalogRightX * 127.0f);
    record->mUnidentified0C[3] = (s8)(input->m_AnalogRightY * 127.0f);
}

extern "C" void fn_803317E0(const InputRouterRecord* record, DetInput* input)
{
    input->m_ButtonBitfield = record->mUnidentified00;
    input->m_LeftTrigger = record->mUnidentified02 & 0xF0;
    input->m_RightTrigger = (record->mUnidentified02 & 0x0F) << 4;
    nlVec3Set(input->m_v3RevRemoteAccel,
        record->mUnidentified03[0] * 0.019665834f,
        record->mUnidentified03[1] * 0.019665834f,
        record->mUnidentified03[2] * 0.019665834f);
    nlVec3Set(input->m_v3RevFreeStyleAccel,
        record->mUnidentified06[0] * 0.019665834f,
        record->mUnidentified06[1] * 0.019665834f,
        record->mUnidentified06[2] * 0.019665834f);
    input->m_nRevDPDNumTargets = record->mUnidentified09;
    nlVec2Set(input->m_v2RevDPDCoord,
        record->mUnidentified0A[0] * 0.007874016f,
        record->mUnidentified0A[1] * 0.007874016f);
    input->m_AnalogLeftX = record->mUnidentified0C[0] / 127.0f;
    input->m_AnalogLeftY = record->mUnidentified0C[1] / 127.0f;
    input->m_AnalogRightX = record->mUnidentified0C[2] / 127.0f;
    input->m_AnalogRightY = record->mUnidentified0C[3] / 127.0f;
}
