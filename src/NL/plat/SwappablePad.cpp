#include "NL/plat/PlatPadManager.h"
#include "NL/plat/SwappablePad.h"
#include "NL/plat/WiiRemotePad.h"
#include "NL/plat/WiiFreestylePad.h"
#include "NL/plat/WiiClassicPad.h"
#include "NL/globalpad.h"
#include "revolution/os/OSInterrupt.h"



bool gEnableWiiRemotePad = true;
bool gEnableWiiFreestylePad = true;
bool gEnableWiiClassicPad = true;

SwappablePadChangedEvent::SwappablePadChangedEvent()
    : UnidentifiedStaticEvent<void(int), 5>("SwappablePadChanged", -1)
{
}

SwappablePadChangedEvent gSwappablePadChanged;

bool UpdatePadBackend(PadBackend* pad)
{
    int type = g_pPlatPadManager->type[pad->m_padIndex];
    if ((type == 1 && !gEnableWiiRemotePad)
        || (type == 2 && !gEnableWiiFreestylePad)
        || (type == 3 && !gEnableWiiClassicPad))
    {
        type = 0;
    }

    int oldType;
    int classID = pad->GetClassID();
    if (classID == gWiiRemotePadClassID)
    {
        oldType = 1;
    }
    else
    {
        classID = pad->GetClassID();
        if (classID == gWiiFreestylePadClassID)
        {
            oldType = 2;
        }
        else
        {
            classID = pad->GetClassID();
            if (classID == gWiiClassicPadClassID)
            {
                oldType = 3;
            }
            else
            {
                classID = pad->GetClassID();
                if (classID == gPlatPadClassID)
                {
                    oldType = 0;
                }
            }
        }
    }

    if (oldType != type)
    {
        OSDisableInterrupts();
        WPADControlMotor(pad->m_padIndex, WPAD_MOTOR_STOP);

        PadBackend* backend = 0;
        switch (type)
        {
        case 1:
            backend = new WiiRemotePad(pad->m_padIndex);
            break;
        case 2:
            backend = new WiiFreestylePad(pad->m_padIndex);
            break;
        case 3:
            backend = new WiiClassicPad(pad->m_padIndex);
            break;
        case 0:
            backend = new cPlatPad(pad->m_padIndex);
            break;
        }

        cGlobalPad* globalPad = g_pPadManager->GetPad(pad->m_padIndex);
        delete globalPad->mBackend;
        globalPad->mBackend = backend;
        OSEnableInterrupts();

        gSwappablePadChanged.Deliver(pad->m_padIndex);
        return true;
    }
    return false;
}
