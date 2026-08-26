#include "NL/nlDebug.h"

#include "types.h"

void nlBreak()
{
    *(u32*)1 = 0;
}
