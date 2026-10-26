#include "NL/gl/glState.h"
#include "Game/Physics/Physics.h"

#include "Game/SharedStaticStorage.h"

bool OnBeforeCreatePhysicsWorld()
{
    return true;
}

static u32 LightTexture = glGetTexture("global/lightramp");
static u32 BlackTexture = glGetTexture("global/black");
static u32 WhiteTexture = glGetTexture("global/white");
