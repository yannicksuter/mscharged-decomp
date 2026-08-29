#include "Game/FE/tlTextInstance.h"

#include "NL/nlString.h"

void TLTextInstance::SetStringId(const char* id)
{
    if (nlStrNCmp<char>(id, "LOC_", 4) == 0)
    {
        id += 4;
    }
    m_LocStrId = nlStringLowerHash(id);
    m_OverloadFlags |= 0x8u;
}
