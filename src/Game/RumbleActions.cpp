#include "Game/RumbleActions.h"

#include "Game/NetworkPeer.h"
#include "NL/globalpad.h"
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

extern unsigned char gRumbleSettings[];
extern RumbleActionState gRumbleActionStates[4];
extern bool gRumbleEnabledOverride;

void PlayRumbleAction(int preset, DetInput* input)
{
    cGlobalPad* pad = 0;
    if (input != 0)
    {
        pad = ((NetworkPeerChannel*)input->m_pMyUser)->GetLocalChannelPad();
    }
    BeginRumbleAction((eRumbleActionPreset)preset, pad);
}

bool IsRumbleActionActive(cGlobalPad* pad)
{
    if (pad == 0)
    {
        return false;
    }
    return gRumbleActionStates[pad->m_padIndex].active;
}

void UpdateRumbleActions(float dt)
{
    if (gRumbleSettings[0xA] || gRumbleEnabledOverride)
    {
        for (int padIndex = 0; padIndex < 4; padIndex++)
        {
            RumbleActionState* state = &gRumbleActionStates[padIndex];
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
