#ifndef GAME_BASIC_STADIUM_H
#define GAME_BASIC_STADIUM_H

#include "Game/World.h"
#include "NL/nlMath.h"

struct HighRangeTweaks;

class BasicStadium : public World
{
public:
    BasicStadium(GLResourcePool* pResource);
    virtual ~BasicStadium();
    virtual DrawableObject* HandleObjectCreation(
        unsigned long uType, WorldObjectLoadContext* pContext);
    virtual void Update(float fDeltaT, bool bUpdateState, bool bUpdateNPCs);

    static BasicStadium* GetCurrentStadium();
    void SetEffectsActive(unsigned long uType, int active);

    /* 0x7C */ nlAVLTree<unsigned long, DrawableObject*,
        DefaultKeyCompare<unsigned long> > m_registeredDrawables;
    /* 0x8C */ nlVector3 m_shadowLightPosition;
    /* 0x98 */ float m_shadowHeight;
    /* 0x9C */ float m_fTime;
    /* 0xA0 */ HighRangeTweaks* m_pHighRangeTweaks;
    /* 0xA4 */ HighRangeTweaks* m_pStadiumHighRangeTweaks;
    /* 0xA8 */ HighRangeTweaks* m_pMegastrikeHighRangeTweaks;
};

typedef char BasicStadium_size_check[sizeof(BasicStadium) == 0xAC ? 1 : -1];

bool SetWorldAnimation(const char* objectName, const char* animationName,
    ePlayMode playMode);
void UpdateHighRange();
void RenderWorldNPCs();
void fn_80278860(BasicStadium* pStadium, int active);
void fn_802789A8(BasicStadium* pStadium, unsigned long uType);

#endif // GAME_BASIC_STADIUM_H
