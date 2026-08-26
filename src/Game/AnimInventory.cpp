#include "Game/AnimInventory.h"

#include "NL/nlMemory.h"

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

cInventory<cSAnim>* g_pDefaultSAnimInventory = 0;

cAnimInventory::cAnimInventory(const AnimProperties* props, int count)
{
    m_nNumProperties = count;
    m_pSAnimInventory = 0;
    m_pSAnims = 0;
    m_pAnimProperties = props;

    m_pSAnimInventory = new (nlMalloc(sizeof(cInventory<cSAnim>), 8, false))
        cInventory<cSAnim>();

    if (g_pDefaultSAnimInventory == 0)
    {
        g_pDefaultSAnimInventory = m_pSAnimInventory;
    }

    m_pSAnims
        = (cSAnim**)nlMalloc((unsigned long)(m_nNumProperties << 2), 8, false);
}

cAnimInventory::~cAnimInventory()
{
    delete m_pSAnimInventory;
    delete[] m_pSAnims;
    g_pDefaultSAnimInventory = 0;
}

void cAnimInventory::AddAnimBundle(
    char* pMem, int len, const char* szFilename)
{
    cInventory<cSAnim>* inventory;
    int i;

    m_pSAnimInventory->AddFile(pMem, len);

    for (i = 0; i < m_nNumProperties; i++)
    {
        inventory = m_pSAnimInventory;
        m_pSAnims[i]
            = inventory->Find((char*)m_pAnimProperties[i].animation);
        if (m_pSAnims[i] == 0)
        {
            nlPrintf("Warning! Could not find \"%s\" in bundle \"%s\"\n",
                m_pAnimProperties[i].animation,
                szFilename);
            inventory = g_pDefaultSAnimInventory;
            m_pSAnims[i]
                = inventory->Find((char*)m_pAnimProperties[i].animation);
            if (m_pSAnims[i] == 0)
            {
                m_pSAnims[i] = m_pSAnims[0];
            }
        }
    }
}
