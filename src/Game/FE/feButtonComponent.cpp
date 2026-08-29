#include "Game/FE/feButtonComponent.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feFontResource.h"
#include "Game/FE/feText.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFont.h"
#include "NL/nlString.h"

ButtonComponent::ButtonComponent()
{
    mButtonInstance = 0;
    mNumButtons = 0;
    mAlreadyCentred = false;
}

ButtonComponent::~ButtonComponent()
{
}

void ButtonComponent::CentreButtons()
{
    float totalLength;
    int i;
    PlatTexture* texture;
    float buttonWidth;
    feVector3 imagePosition;
    feVector3 textPosition;
    unsigned short buffer[128];
    feVector3 labelScale;
    feVector3 componentPosition;

    if (mAlreadyCentred || mButtonInstance == NULL)
    {
        return;
    }

    totalLength = 0.0f;

    for (i = 0; i < mNumButtons; i++)
    {
        int renderedLength;

        texture = glx_GetTex(mButtonImages[i]->m_pTextureResource->GetTextureHandle());
        buttonWidth = (float)texture->m_Width;

        float halfWidth = buttonWidth / 2.0f;
        imagePosition = mButtonImages[i]->GetAssetPosition();
        mButtonImages[i]->SetAssetPosition(totalLength + halfWidth, imagePosition.f.y, imagePosition.f.z);
        totalLength += buttonWidth;

        textPosition = mButtonLabels[i]->GetAssetPosition();
        mButtonLabels[i]->SetAssetPosition(totalLength, textPosition.f.y, textPosition.f.z);

        const nlFont* pFont = ((FEText*)mButtonLabels[i]->m_component)->m_pFeFontResource->m_pFontReference;

        nlStrNCpy<unsigned short>(buffer, mButtonLabels[i]->GetString(), 0x80);
        buffer[127] = 0;

        labelScale = mButtonLabels[i]->GetAssetScale();

        renderedLength = GetRenderedStringLength(buffer, pFont);

        totalLength += (float)(int)(labelScale.f.x * (float)renderedLength);
        totalLength += 32.0f;
    }

    totalLength -= 32.0f;

    componentPosition = mButtonInstance->GetAssetPosition();
    mButtonInstance->SetAssetPosition(-(totalLength / 2.0f), componentPosition.f.y, componentPosition.f.z);
    mButtonInstance->m_bVisible = true;
    mAlreadyCentred = true;
}

int ButtonComponent::GetRenderedStringLength(const unsigned short* pString, const nlFont* pFont)
{
    int returnValue;
    unsigned char firstChar;
    const unsigned short* pLastChar;
    unsigned short* pCurrentChar;
    unsigned long charWidth;

    returnValue = 0;
    firstChar = true;
    pLastChar = 0;
    {
        FontCharString fcs(pString, pFont, (unsigned short*)0);
        pCurrentChar = fcs.m_pString;

        while (*pCurrentChar != 0)
        {
            charWidth = pFont->GetCharWidth(*pCurrentChar, firstChar ? 0 : *pLastChar);
            returnValue += charWidth;
            pLastChar = pCurrentChar;
            firstChar = false;
            pCurrentChar++;
        }
    }

    return returnValue;
}

void ButtonComponent::SetState(ButtonComponent::ButtonState buttonstate)
{
    if (mButtonInstance != NULL)
    {
        mAlreadyCentred = false;

        switch (buttonstate)
        {
        case BS_A_AND_B:
            mButtonInstance->SetActiveSlide("a and b", true, false);
            mNumButtons = 2;
            mButtonImages[1] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "A_button");
            mButtonLabels[1] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "accept");
            mButtonImages[0] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "B_button");
            mButtonLabels[0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "back");
            break;

        case BS_A_ONLY:
            mButtonInstance->SetActiveSlide("a", true, false);
            mNumButtons = 1;
            mButtonImages[0] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "A_button");
            mButtonLabels[0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "accept");
            break;

        case BS_B_ONLY:
            mButtonInstance->SetActiveSlide("b", true, false);
            mNumButtons = 1;
            mButtonImages[0] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "B_button");
            mButtonLabels[0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "back");
            break;

        case BS_A_AND_B_AND_Y:
            mButtonInstance->SetActiveSlide("a b y", true, false);
            mNumButtons = 3;
            mButtonImages[1] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "A_button");
            mButtonLabels[1] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "accept");
            mButtonImages[0] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "B_button");
            mButtonLabels[0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "back");
            mButtonImages[2] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "y_button");
            mButtonLabels[2] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "OPTIONS");
            break;

        case 4:
            mButtonInstance->SetActiveSlide("a and b and start", true, false);
            mNumButtons = 3;
            mButtonImages[0] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "start_button");
            mButtonLabels[0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "start");
            mButtonImages[1] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "B_button");
            mButtonLabels[1] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "back");
            mButtonImages[2] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "A_button");
            mButtonLabels[2] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(mButtonInstance->GetActiveSlide(), "accept");
            break;

        default:
            break;
        }
    }
}
