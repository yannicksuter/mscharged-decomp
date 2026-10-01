#include "Game/FE/tlDefault.h"

template <class TInstance, class TObject>
TObject TLDefault<TInstance, TObject>::sObject;

template <class TInstance, class TObject>
TInstance TLDefault<TInstance, TObject>::sInstance(&sObject);

template <class TObject, int N>
TObject TLDefaultObject<TObject, N>::sObject;

template struct TLDefault<TLComponentInstance, TLComponent>;
template struct TLDefault<TLGroupInstance, FEGroup>;
template struct TLDefault<TLImageInstance, FEImage>;
template struct TLDefault<TLLayerInstance, FELayer>;
template struct TLDefault<TLTextInstance, FEText>;
template struct TLDefault<TLInstance, FEGroup>;
template struct TLDefaultObject<TLSlide, 0>;
template struct TLDefaultObject<TLSlide, 1>;
