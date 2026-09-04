#ifndef GAME_RENDER_ATTACK_SIDE_INDICATORS_H
#define GAME_RENDER_ATTACK_SIDE_INDICATORS_H

class DrawableObject;

void CreateAttackSideIndicators();
void DestroyAttackSideIndicators();
void UpdateAttackSideIndicators();
void RegisterAttackSideIndicator(DrawableObject* object);

#endif // GAME_RENDER_ATTACK_SIDE_INDICATORS_H
