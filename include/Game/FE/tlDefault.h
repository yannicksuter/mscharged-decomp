#ifndef GAME_FE_TL_DEFAULT_H
#define GAME_FE_TL_DEFAULT_H

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
// Game/FE/tlDefault.cpp; every other unit only takes their addresses.
struct TLGroupInstance : public TLInstance
{
    TLGroupInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_type = TLAT_GROUP;
    }
};

struct TLLayerInstance : public TLInstance
{
    TLLayerInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_type = TLAT_LAYER;
    }
};

template <class TInstance, class TObject>
struct TLDefault
{
    static TObject sObject;
    static TInstance sInstance;
};

template <class TObject, int N>
struct TLDefaultObject
{
    static TObject sObject;
};

typedef TLDefault<TLComponentInstance, TLComponent>
    TLComponentDefault;
typedef TLDefault<TLGroupInstance, FEGroup>
    TLGroupDefault;
typedef TLDefault<TLImageInstance, FEImage>
    TLImageDefault;
typedef TLDefault<TLLayerInstance, FELayer>
    TLLayerDefault;
typedef TLDefault<TLTextInstance, FEText> TLTextDefault;

#endif // GAME_FE_TL_DEFAULT_H
