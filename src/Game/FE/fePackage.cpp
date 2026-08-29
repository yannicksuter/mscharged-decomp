#include "Game/FE/fePackage.h"

#include "Game/FE/fePresentation.h"

FEPresentation* FEPackage::GetPresentation() const
{
    return m_pFEPresentation;
}

void FEPackage::Update(float deltaTime)
{
    m_pFEPresentation->Update(deltaTime);
}
