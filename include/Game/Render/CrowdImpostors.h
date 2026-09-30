#ifndef GAME_RENDER_CROWD_IMPOSTORS_H
#define GAME_RENDER_CROWD_IMPOSTORS_H

#include "Game/Render/CrowdModelCollection.h"
#include "Game/Render/ImpostorCharacter.h"
#include "NL/nlMemory.h"

class CrowdImpostorCharacter
    : public AnimatedImpostorCharacter
{
public:
    CrowdImpostorCharacter(const char* name,
        ImpostorModel* model, void* animation, int budget,
        int numAngles, int numTextures,
        const ImpostorCharacterParams* params)
        : AnimatedImpostorCharacter(name, model, animation, budget,
            numAngles, numTextures, params)
    {
    }
    virtual ~CrowdImpostorCharacter();
};

extern CrowdModelCollection gCrowdModelCollection;

void UpdateImpostorPositions();
void LoadCrowdCharacterList();
void InitializeCrowdImpostors(bool alternateView);
void UninitializeCrowdImpostors();
void CreateCrowdLayoutObject();
void SetCrowdModelTexture(unsigned int textureHash, unsigned long texture);
void SetCrowdImpostorsExcited();
void SetCrowdImpostorsIdle();
void UpdateCrowdImpostorAnimation(float value);

#endif // GAME_RENDER_CROWD_IMPOSTORS_H
