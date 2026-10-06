#ifndef NL_PLAT_SOCKET_NETWORK_H
#define NL_PLAT_SOCKET_NETWORK_H

void SocketNetworkInitializeMemory();
void SocketNetworkStartup();
void SocketNetworkShutdown();
bool SocketNetworkIsStarted();
int SocketNetworkGetLastError();
void SocketNetworkStartupAsync();
bool SocketNetworkIsStartupComplete();

#endif // NL_PLAT_SOCKET_NETWORK_H
