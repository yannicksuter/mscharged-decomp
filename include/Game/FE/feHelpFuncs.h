#ifndef GAME_FE_FEHELPFUNCS_H
#define GAME_FE_FEHELPFUNCS_H

class TLTextInstance;

void MakeTextBoxReallyWide(TLTextInstance& textInstance);
void EnableAutoPressed();

namespace TakeGameMemSnapshot
{
extern unsigned char gTakenSnapshot;
extern float gTimeElapsed;

void WriteToDisk();
void ResetTimers();
void Update(float dt);
} // namespace TakeGameMemSnapshot

#endif // GAME_FE_FEHELPFUNCS_H
