#include "Game/TextWindowTask.h"

#include "types.h"

struct InputDevice
{
    virtual void Unknown0();
    virtual bool IsActive();
    virtual bool CheckHeld(u32 action, bool consume);
    virtual void Unknown3();
    virtual void Unknown4();
    virtual bool CheckPressed(u32 action, bool consume);
};

extern void* lbl_806E1E28;

extern "C" InputDevice* fn_802C082C(void* manager, int index);

TextWindowTask* TextWindowTask::sInstance;

extern "C" void fn_802BD820(const char*, ...)
{
}

void TextWindowTask::Run(float)
{
    if (!mActive)
    {
        return;
    }

    InputDevice* input;
    int index = 0;
    do
    {
        input = fn_802C082C(lbl_806E1E28, index);
        if (input->IsActive())
        {
            break;
        }
        ++index;
    } while (index < 4);

    int direction = 0;
    if (input->CheckPressed(mPrevious, true) && mPrevious != 0xFFFFFFFF)
    {
        direction = -1;
    }
    if (input->CheckPressed(mNext, true) && mNext != 0xFFFFFFFF)
    {
        direction = 1;
    }

    if (direction == 0)
    {
        if (input->CheckPressed(mPreviousAlternate, true) && mPreviousAlternate != 0xFFFFFFFF)
        {
            direction = -1;
        }
        if (input->CheckPressed(mNextAlternate, true) && mNextAlternate != 0xFFFFFFFF)
        {
            direction = 1;
        }
    }
    else
    {
        input->CheckHeld(mHeld, true);
    }

    if (direction == 0)
    {
        input->CheckPressed(mSelect, true);
    }
}
