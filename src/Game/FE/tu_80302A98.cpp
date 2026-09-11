#include "Game/FE/UnidentifiedTLDefault.h"

template <class TInstance, class TObject>
TObject UnidentifiedTLDefault<TInstance, TObject>::sObject;

template <class TInstance, class TObject>
TInstance UnidentifiedTLDefault<TInstance, TObject>::sInstance(&sObject);

template <class TObject, int N>
TObject UnidentifiedTLDefaultObject<TObject, N>::sObject;

template struct UnidentifiedTLDefault<TLComponentInstance, TLComponent>;
template struct UnidentifiedTLDefault<UnidentifiedTLGroupInstance, FEGroup>;
template struct UnidentifiedTLDefault<TLImageInstance, FEImage>;
template struct UnidentifiedTLDefault<UnidentifiedTLLayerInstance, FELayer>;
template struct UnidentifiedTLDefault<TLTextInstance, FEText>;
template struct UnidentifiedTLDefault<TLInstance, FEGroup>;
template struct UnidentifiedTLDefaultObject<TLSlide, 0>;
template struct UnidentifiedTLDefaultObject<TLSlide, 1>;
