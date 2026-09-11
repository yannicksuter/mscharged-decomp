#include "Game/AI/AISandbox.h"

#include "Game/UnidentifiedStaticStorage.h"

template <>
AISandbox* nlSingleton<AISandbox>::s_pInstance = 0;
