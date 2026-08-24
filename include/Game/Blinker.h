#ifndef GAME_BLINKER_H
#define GAME_BLINKER_H

class glModel;

class Blinker
{
public:
    void Blink(glModel* model);
    void Update(float fDeltaT);
};

#endif // GAME_BLINKER_H
