#include "NL/gl/glState.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" bool fn_8013E2E4()
{
    return true;
}

static u32 LightTexture = glGetTexture("global/lightramp");
static u32 BlackTexture = glGetTexture("global/black");
static u32 WhiteTexture = glGetTexture("global/white");
