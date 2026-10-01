#ifndef GAME_FE_FE_IMPOSTOR_CHARACTER_H
#define GAME_FE_FE_IMPOSTOR_CHARACTER_H

#include "Game/Render/ImpostorCharacter.h"
#include "Game/TweakValue.h"
#include "types.h"

// Defined apart from feModelManager.cpp and ahead of the model classes:
// R4QE01 keeps the empty UpdateAnimation behind the unit's list-container
// DeleteEntry copies, and this vtable behind the manager's and the models'.
// The original file and its name are not recoverable.
class FEImpostorCharacter
    : public AnimatedImpostorCharacter
{
public:
    FEImpostorCharacter(const char* name,
        ImpostorModel* model, void* animations, int budget,
        bool animationFlag, bool alternate,
        const ImpostorCharacterParams* params, int modelType);
    virtual ~FEImpostorCharacter();

    virtual void SetScale(float scale);
    virtual float GetScale();
    virtual float GetCameraDistance();
    virtual float GetCameraLookatZ();
    virtual void Render(GLView* target, int texture);
    virtual void UpdateAnimation(float dt) { }

    /* 0x74 */ bool mEnabled;
    /* 0x75 */ u8 mPadding75[3];
    /* 0x78 */ int mModelType;
    /* 0x7C */ TweakFloatBinding mfScaleInitialCup;
    /* 0x8C */ TweakFloatBinding mfScaleCup;
    /* 0x9C */ TweakFloatBinding mfCameraLookatZInitialCup;
    /* 0xAC */ TweakFloatBinding mfCameraLookatZCup;
    /* 0xBC */ TweakFloatBinding mfCameraDistanceInitialCup;
    /* 0xCC */ TweakFloatBinding mfCameraDistanceCup;
}; // size: 0xDC

#endif // GAME_FE_FE_IMPOSTOR_CHARACTER_H
