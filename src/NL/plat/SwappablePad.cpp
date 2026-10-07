#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/EventBase.inl"
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
    if ((type == PLAT_PAD_REMOTE && !gEnableWiiRemotePad)
        || (type == PLAT_PAD_FREESTYLE && !gEnableWiiFreestylePad)
        || (type == PLAT_PAD_CLASSIC && !gEnableWiiClassicPad))
    {
        type = PLAT_PAD_NONE;
    }

    int oldType;
    int classID = pad->GetClassID();
    if (classID == gWiiRemotePadClassID)
    {
        oldType = PLAT_PAD_REMOTE;
    }
    else
    {
        classID = pad->GetClassID();
        if (classID == gWiiFreestylePadClassID)
        {
            oldType = PLAT_PAD_FREESTYLE;
        }
        else
        {
            classID = pad->GetClassID();
            if (classID == gWiiClassicPadClassID)
            {
                oldType = PLAT_PAD_CLASSIC;
            }
            else
            {
                classID = pad->GetClassID();
                if (classID == gPlatPadClassID)
                {
                    oldType = PLAT_PAD_NONE;
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
        case PLAT_PAD_REMOTE:
            backend = new WiiRemotePad(pad->m_padIndex);
            break;
        case PLAT_PAD_FREESTYLE:
            backend = new WiiFreestylePad(pad->m_padIndex);
            break;
        case PLAT_PAD_CLASSIC:
            backend = new WiiClassicPad(pad->m_padIndex);
            break;
        case PLAT_PAD_NONE:
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
