#include "Game/AI/DesireRunToNet.h"

#include "Game/AI/Fielder.h"
#include <stddef.h>
#include "Game/AI/SpaceSearch.h"
#include "Game/DebugWriteCache.h"
#include "Game/Field.h"
#include "Game/Player.h"
#include "NL/nlMemory.h"

#include "Game/UnidentifiedStaticStorage.h"


bool lbl_806E0E50;

static float lbl_806DC1F8 = 5.0f;
static float lbl_806DC1FC = 5.0f;
static unsigned short sDesireRunToNetType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800C3D9C | size: 0x148
 */
bool DesireRunToNet::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);

    m_pSpaceSearch = new (8, false) SSearchRunToNet(m_pFielder);
    m_pFielder->SetSpaceSearch(m_pSpaceSearch);
    m_pFielder->m_pSpaceSearch->m_bDebugOn = lbl_806E0E50;

    nlVector3 v3BestPosition;
    m_pFielder->m_pSpaceSearch->FindBestPosition(
        v3BestPosition, m_pFielder->mUnidentified024.m_v3Position,
        DIR_NONE, NULL, lbl_806DC1F8, 0x8000);

    nlVector3 v3DesiredVelDirection;
    v3DesiredVelDirection.Sub2D(
        v3BestPosition, m_pFielder->mUnidentified024.m_v3Position);
    v3DesiredVelDirection.z = 0.0f;

    float fLengthSq = v3DesiredVelDirection.GetLengthSq3D();
    if (fLengthSq == 0.0f)
    {
        nlPolarToCartesian(v3DesiredVelDirection.x,
            v3DesiredVelDirection.y,
            m_pFielder->mUnidentified024.m_aDesiredFacingDirection, 1.0f);
    }
    else
    {
        float fInvDistance = nlRecipSqrt(fLengthSq, true);
        nlVec3Scale(v3DesiredVelDirection, fInvDistance);
    }

    mvDesiredPosition = v3DesiredVelDirection;
    return result;
}

/**
 * Offset/Address/Size: 0x148 | 0x800C3EE4 | size: 0x84
 */
void DesireRunToNet::Update(
    DesireUpdate*, float)
{
    nlVector3 v3DesiredPosition;
    nlVec3ScaleAdd(v3DesiredPosition, lbl_806DC1FC,
        mvDesiredPosition, m_pFielder->mUnidentified024.m_v3Position);
    cField::FixOutOfBoundsPosition(v3DesiredPosition, 0.2f, true);
    m_pFielder->AddDesiredPosition(v3DesiredPosition, 1.5f, 1.0f);
}

/**
 * Offset/Address/Size: 0x1CC | 0x800C3F68 | size: 0x50
 */
void DesireRunToNet::UnidentifiedCleanup()
{
    if (m_pSpaceSearch == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(NULL);
    }
    m_pSpaceSearch = NULL;
}

/**
 * Offset/Address/Size: 0x21C | 0x800C3FB8 | size: 0xC8
 */
inline void DesireRunToNet::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunToNet");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x2E4 | 0x800C4080 | size: 0x9C
 */
inline void DesireRunToNet::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireRunToNetType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireRunToNetType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireRunToNetType, data, context);
    cache->WriteData(sDesireRunToNetType, data,
        sizeof(DesireRunToNet) - offset);
}

/**
 * Offset/Address/Size: 0x380 | 0x800C411C | size: 0x5C
 */
inline DesireRunToNet::~DesireRunToNet()
{
}
