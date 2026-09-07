#include "NL/plat/PlatPadManager.h"
#include "NL/plat/tu_80365E84.h"
#include "NL/plat/tu_803648EC.h"
#include "NL/plat/tu_80364E5C.h"
#include "NL/plat/tu_80365488.h"
#include "NL/globalpad.h"
#include "revolution/os/OSInterrupt.h"

extern int lbl_806E2268;
extern int lbl_806E227C;
extern int lbl_806E228C;
extern int lbl_806E229C;

bool lbl_806DFA48 = true;
bool lbl_806DFA49 = true;
bool lbl_806DFA4A = true;
Class_80365E84 lbl_8058936C;

extern "C" bool fn_80365E84(PadBackend* pad)
{
    int type = g_pPlatPadManager->type[pad->m_padIndex];
    if ((type == 1 && !lbl_806DFA48)
        || (type == 2 && !lbl_806DFA49)
        || (type == 3 && !lbl_806DFA4A))
    {
        type = 0;
    }

    int oldType;
    if (pad->UnidentifiedClassID() == lbl_806E227C)
    {
        oldType = 1;
    }
    else if (pad->UnidentifiedClassID() == lbl_806E228C)
    {
        oldType = 2;
    }
    else if (pad->UnidentifiedClassID() == lbl_806E229C)
    {
        oldType = 3;
    }
    else if (pad->UnidentifiedClassID() == lbl_806E2268)
    {
        oldType = 0;
    }

    if (oldType != type)
    {
        OSDisableInterrupts();
        WPADControlMotor(pad->m_padIndex, WPAD_MOTOR_STOP);

        PadBackend* backend = 0;
        switch (type)
        {
        case 1:
            backend = new Class_803648EC(pad->m_padIndex);
            break;
        case 2:
            backend = new Class_80364E5C(pad->m_padIndex);
            break;
        case 3:
            backend = new Class_80365488(pad->m_padIndex);
            break;
        case 0:
            backend = new cPlatPad(pad->m_padIndex);
            break;
        }

        cGlobalPad* globalPad = g_pPadManager->GetPad(pad->m_padIndex);
        delete globalPad->mBackend;
        globalPad->mBackend = backend;
        OSEnableInterrupts();

        lbl_8058936C.UnidentifiedDeliver(pad->m_padIndex);
        return true;
    }
    return false;
}
