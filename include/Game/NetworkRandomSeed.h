#ifndef GAME_NETWORK_RANDOM_SEED_H
#define GAME_NETWORK_RANDOM_SEED_H

#include "types.h"

extern u32 gNetworkRandomSeed;

u32 GetNetworkRandomSeed();
void SetNetworkRandomSeed(u32 seed);
void OnInputSessionReset();

#endif // GAME_NETWORK_RANDOM_SEED_H
