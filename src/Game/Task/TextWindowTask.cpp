#include "Game/Task/TextWindowTask.h"

#include "NL/globalpad.h"
#include "types.h"

TextWindowTask* TextWindowTask::sInstance;

void fn_802BD820(const char*, ...)
{
}

void TextWindowTask::Run(float)
{
    if (!mActive)
    {
        return;
    }

    cGlobalPad* input;
    int index = 0;
    do
    {
        input = fn_802C082C(lbl_806E1E28, index);
        if (input->IsConnected())
        {
            break;
        }
        ++index;
    } while (index < 4);

    int direction = 0;
    if (input->PlatJustPressed(mPrevious, true) && mPrevious != 0xFFFFFFFF)
    {
        direction = -1;
    }
    if (input->PlatJustPressed(mNext, true) && mNext != 0xFFFFFFFF)
    {
        direction = 1;
    }

    if (direction == 0)
    {
        if (input->PlatJustPressed(mPreviousAlternate, true) && mPreviousAlternate != 0xFFFFFFFF)
        {
            direction = -1;
        }
        if (input->PlatJustPressed(mNextAlternate, true) && mNextAlternate != 0xFFFFFFFF)
        {
            direction = 1;
        }
    }
    else
    {
        input->IsPressed(mHeld, true);
    }

    if (direction == 0)
    {
        input->PlatJustPressed(mSelect, true);
    }
}
