#include "NL/nlMain.h"

#include "Game/SAnim.h"
#include "NL/gl/glPlat.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"
#include "NL/nlTime.h"

void nlInit()
{
    nlInitMemory();
    glplatPreStartup();
    nlInitTicker();
    nlInitTime();
    nlInitRandom();
    nlInitFileSystem();
    SAnimInitGQR();
}
