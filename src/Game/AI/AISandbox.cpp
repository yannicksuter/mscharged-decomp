#include "Game/AI/AISandbox.h"

#include "Game/SharedStaticStorage.h"

template <>
AISandbox* nlSingleton<AISandbox>::s_pInstance = 0;
