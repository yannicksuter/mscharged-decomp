#ifndef GAME_NETWORK_DIAGNOSTICS_H
#define GAME_NETWORK_DIAGNOSTICS_H

void FormatNetworkTimestamp(char* text, unsigned long size, bool arg2);
int FormatNetworkCallStack(int maxDepth, char* buffer, int size);

#endif // GAME_NETWORK_DIAGNOSTICS_H
