#ifndef GAME_FE_UNIDENTIFIED_TL_DEFAULT_H
#define GAME_FE_UNIDENTIFIED_TL_DEFAULT_H

#include "Game/FE/feGroup.h"
#include "Game/FE/feImage.h"
#include "Game/FE/feLayer.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"

// The default instance of each timeline asset type together with the library
// object it wraps. The static members are defined once, in
// Game/FE/tu_80302A98.cpp; every other unit only takes their addresses.
struct UnidentifiedTLGroupInstance : public TLInstance
{
    UnidentifiedTLGroupInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_type = TLAT_GROUP;
    }
};

struct UnidentifiedTLLayerInstance : public TLInstance
{
    UnidentifiedTLLayerInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_type = TLAT_LAYER;
    }
};

template <class TInstance, class TObject>
struct UnidentifiedTLDefault
{
    static TObject sObject;
    static TInstance sInstance;
};

template <class TObject, int N>
struct UnidentifiedTLDefaultObject
{
    static TObject sObject;
};

typedef UnidentifiedTLDefault<TLComponentInstance, TLComponent>
    UnidentifiedTLComponentDefault;
typedef UnidentifiedTLDefault<UnidentifiedTLGroupInstance, FEGroup>
    UnidentifiedTLGroupDefault;
typedef UnidentifiedTLDefault<TLImageInstance, FEImage>
    UnidentifiedTLImageDefault;
typedef UnidentifiedTLDefault<UnidentifiedTLLayerInstance, FELayer>
    UnidentifiedTLLayerDefault;
typedef UnidentifiedTLDefault<TLTextInstance, FEText> UnidentifiedTLTextDefault;

#endif // GAME_FE_UNIDENTIFIED_TL_DEFAULT_H
