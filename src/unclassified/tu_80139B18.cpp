#include "Game/RumbleActions.h"

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

extern unsigned char lbl_8056FEAC[];
extern RumbleActionState lbl_8056FEB8[4];
extern bool lbl_806E11A0;
extern void* lbl_806E1E28;

extern "C" cGlobalPad* fn_802C082C(void* manager, int index);

void UpdateRumbleActions(float dt)
{
    if (lbl_8056FEAC[0xA] || lbl_806E11A0)
    {
        for (int padIndex = 0; padIndex < 4; padIndex++)
        {
            RumbleActionState* state = &lbl_8056FEB8[padIndex];
            if (state->active != 0)
            {
                cGlobalPad* pad = fn_802C082C(lbl_806E1E28, padIndex);

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
