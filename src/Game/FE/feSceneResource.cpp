#include "Game/FE/feSceneResource.h"

FESceneResource::FESceneResource()
{
    m_next = 0;
    m_prev = 0;
    m_bValid = false;
    field_0x14 = 0;
    m_pFESceneContext = 0;
    m_type = FERT_SCENE;
}
