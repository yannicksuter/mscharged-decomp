#include "Game/FE/feMenu.h"

class TLComponentInstance;

// Stub needed to emit this unit's weak MenuItem/MenuList/Function<void(TLComponentInstance*)>
// copies and the MenuList vtable in the original order. The translation unit that emitted
// them is unidentified and its own code was dead-stripped. InstantiateMenuList never existed
// in the game; it is unreferenced, so the linker strips it and the DOL still matches.
template <typename T>
void InstantiateMenuList()
{
    MenuList<T> list;
}

template void InstantiateMenuList<TLComponentInstance>();
