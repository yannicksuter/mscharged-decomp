#ifndef UNCLASSIFIED_TU_80219248_H
#define UNCLASSIFIED_TU_80219248_H

#include "unclassified/tu_80300104.h"

class TU80219248Component : public TU80300104Component
{
public:
    TU80219248Component();
    TU80219248Component(void* context);

    bool fn_802192FC(int value, unsigned int which) const;
    void fn_802195B4(int index);
    void fn_80219608(const TU80300104Event* event);

    virtual void fn_80301BE8(int index, void* context);
    virtual void fn_80301BA8(int index, void* context);
    virtual void fn_80301CE8(int index, void* context);
    virtual void fn_80301C28(int index, void* context);
    virtual void fn_80301C68(int index, void* context);
    virtual void fn_80301D28(int index, void* context);
    virtual void fn_80301CA8(int index, void* context);

    /* 0xA0 */ int mValues[4];
    /* 0xB0 */ bool mSpeakerEnabled;
}; // size 0xB4

#endif // UNCLASSIFIED_TU_80219248_H
