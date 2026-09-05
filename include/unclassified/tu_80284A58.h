#ifndef UNCLASSIFIED_TU_80284A58_H
#define UNCLASSIFIED_TU_80284A58_H

#include "types.h"

// Singleton returned by fn_80284A58. Only the fields this unit reads are
// modelled; the letter-box names come from the predecessor's Presentation.
struct UnidentifiedPresentationState
{
    char mUnidentified000[0xC0];
    /* 0xC0 */ float mLetterBoxDuration;
    /* 0xC4 */ bool mLetterBoxEnabled;
    char mUnidentified0C5[0x9F];
    /* 0x164 */ bool mUnidentified164;
};

UnidentifiedPresentationState* fn_80284A58();
void fn_80285714(UnidentifiedPresentationState* state, u32 from, u32 to);
bool fn_80287AB0(UnidentifiedPresentationState* state);

#endif // UNCLASSIFIED_TU_80284A58_H
