#ifndef GAME_RENDER_CROWD_IMPOSTORS_H
#define GAME_RENDER_CROWD_IMPOSTORS_H

#include "Game/Render/CrowdModelCollection.h"
#include "Game/Render/ImpostorCharacter.h"
#include "NL/nlMemory.h"

class CrowdModelArray
{
public:
    CrowdModelArray(int count)
    {
        data = new (8, false) ImpostorModel*[count];
        capacity = count;
    }
    ~CrowdModelArray();

    ImpostorModel*& operator[](unsigned int index) { return data[index]; }

    ImpostorModel** data;
    int capacity;
};

class CrowdCharacterArray
{
public:
    CrowdCharacterArray(int count)
    {
        data = new (8, false) AnimatedImpostorCharacter*[count];
        capacity = count;
    }
    ~CrowdCharacterArray();

    AnimatedImpostorCharacter*& operator[](unsigned int index)
    {
        return data[index];
    }

    AnimatedImpostorCharacter** data;
    int capacity;
};

class CrowdDefinitionArray
{
public:
    CrowdDefinitionArray(int count)
    {
        data = new (8, false) CrowdCharacterDefinition[count];
        capacity = count;
    }
    ~CrowdDefinitionArray();

    CrowdCharacterDefinition* data;
    int capacity;
};


void UpdateImpostorPositions();
void LoadCrowdCharacterList();
void InitializeCrowdImpostors(bool alternateView);
void UninitializeCrowdImpostors();
void CreateCrowdLayoutObject();
void SetCrowdModelTexture(unsigned int hash, unsigned long texture);
void SetCrowdImpostorsExcited();
void SetCrowdImpostorsIdle();
void UpdateCrowdImpostorAnimation(float value);

#endif // GAME_RENDER_CROWD_IMPOSTORS_H
