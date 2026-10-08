#include "Game/FE/feMenu.h"

class TLComponentInstance;

// This explicit instantiation supplies this unit's MenuItem/MenuList/Function
// weak definitions and MenuList vtable. The unreferenced wrapper is discarded
// at link time; the originating source use and owner remain unresolved.
template <typename T>
void InstantiateMenuList()
{
    MenuList<T> list;
}

template void InstantiateMenuList<TLComponentInstance>();
