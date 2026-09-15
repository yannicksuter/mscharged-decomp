#include "revolution/wpad/WPAD.h"

#include "Game/RumbleActions.h"

#include "Game/NetworkPeer.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/globalpad.h"
#include "NL/nlConfig.h"
#include "types.h"

struct RumbleOp
{
    s32 type;
    u32 value;
};

struct RumbleActionState
{
    bool active;
    char pad_0x01[3];
    RumbleOp* ops;
    u32 current;
    bool pending;
    char pad_0x0D[3];
    float timer;
};

RumbleOp opArrayShotContact[] = {
    { 0, 0x29A },
    { 2, 0 },
};

RumbleOp opArraySolidContact[] = {
    { 0, 0x1BC },
    { 2, 0 },
};

RumbleOp opArrayMediumContact[] = {
    { 0, 0xDE },
    { 2, 0 },
};

RumbleOp opArraySmallContact[] = {
    { 0, 0x6F },
    { 2, 0 },
};

RumbleOp lbl_8050DFF8[] = {
    { 0, 0x2D },
    { 1, 0x96 },
    { 2, 0 },
};

static TweakValueBool g_bRumbleOn(
    "g_bRumbleOn", "Controller Config", true);
static RumbleActionState rumbleActionState[4];
bool gRumbleEnabledOverride;

void UpdateRumbleActions(float dt)
{
    if (g_bRumbleOn || gRumbleEnabledOverride)
    {
        for (int padIndex = 0; padIndex < 4; padIndex++)
        {
            RumbleActionState* state = &rumbleActionState[padIndex];
            if (state->active != 0)
            {
                cGlobalPad* pad = g_pPadManager->GetPad(padIndex);

                if (state->pending != 0)
                {
                    state->timer -= dt;
                    if (!(state->timer <= 0.0f))
                    {
                        continue;
                    }
                    state->current += 1;
                    state->pending = 0;
                }

                int currentOp = state->current;
                RumbleOp* op = &state->ops[currentOp];

                switch (op->type)
                {
                case 0:
                    if (op->value != 0)
                    {
                        state->timer = (float)op->value / 1000.0f;
                        state->pending = 1;
                        pad->StartRumble(state->timer, 0.0f, 0.0f);
                    }
                    else
                    {
                        state->current++;
                    }
                    break;

                case 1:
                    pad->StopRumble();
                    {
                        int nextOp = state->current;
                        u32 delayValue = state->ops[nextOp].value;
                        if (delayValue != 0)
                        {
                            state->timer = (float)delayValue / 1000.0f;
                            state->pending = true;
                        }
                        else
                        {
                            state->current++;
                        }
                    }
                    break;

                case 2:
                    pad->StopRumble();
                    state->active = 0;
                    state->ops = 0;
                    break;
                }
            }
        }
    }
}

void PlayRumbleAction(int preset, DetInput* input)
{
    cGlobalPad* pad = 0;
    if (input != 0)
    {
        pad = ((NetworkPeerChannel*)input->m_pMyUser)->GetLocalChannelPad();
    }
    BeginRumbleAction((eRumbleActionPreset)preset, pad);
}

static inline void StopRumble(cGlobalPad* pad)
{
    if ((g_bRumbleOn || gRumbleEnabledOverride) && pad != 0)
    {
        int idx = pad->m_padIndex;
        pad->StopRumble();

        rumbleActionState[idx].active = false;
        rumbleActionState[idx].pending = false;
        rumbleActionState[idx].ops = 0;
        rumbleActionState[idx].current = 0;
    }
}

void BeginRumbleAction(eRumbleActionPreset preset, cGlobalPad* pad)
{
    g_bRumbleOn
        = WPADIsMotorEnabled() && preset != RUMBLE_POINTER_HOVER;
    gRumbleEnabledOverride
        = WPADIsMotorEnabled() && preset == RUMBLE_POINTER_HOVER;

    if (!Config::Global().Get<bool>("rumble", true))
    {
        g_bRumbleOn = false;
    }

    if (preset == RUMBLE_POINTER_HOVER
        && !Config::Global().Get<bool>("fe_rumble", true))
    {
        gRumbleEnabledOverride = false;
    }

    if ((g_bRumbleOn || gRumbleEnabledOverride) && pad != 0)
    {
        RumbleOp* ops;
        int idx = pad->m_padIndex;

        switch (preset)
        {
        case 0:
            StopRumble(pad);
            return;
        case RUMBLE_SMALL_CONTACT:
            ops = opArraySmallContact;
            break;
        case RUMBLE_MEDIUM_CONTACT:
            ops = opArrayMediumContact;
            break;
        case RUMBLE_SOLID_CONTACT:
            ops = opArraySolidContact;
            break;
        case RUMBLE_SHOT_CONTACT:
            ops = opArrayShotContact;
            break;
        case RUMBLE_POINTER_HOVER:
            ops = lbl_8050DFF8;
            break;
        default:
            return;
        }

        rumbleActionState[idx].active = true;
        rumbleActionState[idx].pending = false;
        rumbleActionState[idx].current = 0;
        rumbleActionState[idx].ops = ops;
    }
}

void StopRumbleAction(cGlobalPad* pad)
{
    StopRumble(pad);
}

bool IsRumbleActionActive(cGlobalPad* pad)
{
    if (pad == 0)
    {
        return false;
    }
    return rumbleActionState[pad->m_padIndex].active;
}
