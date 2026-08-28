#ifndef GAME_TWEAKS_BASE_H
#define GAME_TWEAKS_BASE_H

class TweaksBase
{
public:
    TweaksBase(const char* param_1);
    virtual ~TweaksBase();
    virtual void Init() = 0;

protected:
    /* 0x04 */ char mszFileName[0x40];
}; // total size: 0x44

#endif // GAME_TWEAKS_BASE_H
