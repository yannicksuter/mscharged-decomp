#ifndef GAME_EFFECTS_EMISSION_MANAGER_H
#define GAME_EFFECTS_EMISSION_MANAGER_H

class EffectsGroup;

class EmissionManager
{
public:
    static EmissionManager* Instance();
    void Destroy(unsigned long userData, const EffectsGroup* pEffectsGroup);
    void DestroyAll(int unknown, bool exceptPersistent);
    void DestroyAll(bool exceptPersistent);
    void Kill(unsigned long userData, const EffectsGroup* pEffectsGroup);
    bool IsPlaying(
        unsigned long userData, const EffectsGroup* pEffectsGroup);
};

extern "C" void fn_802E7720(void*, float);
extern "C" void fn_802E7B30(void*);
extern "C" void fn_802E6DF8(void*);
extern "C" void fn_802E6C20(void*, void*, int, int);
extern "C" void fn_802E6620(bool, bool, bool, bool);
extern "C" bool fn_802E6774(void*);

#endif // GAME_EFFECTS_EMISSION_MANAGER_H
