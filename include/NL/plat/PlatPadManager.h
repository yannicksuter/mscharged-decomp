#ifndef NL_PLAT_PLATPADMANAGER_H
#define NL_PLAT_PLATPADMANAGER_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/platpad.h"
#include "Game/Event.h"

class PadDeviceChangedEvent : public UnidentifiedStaticEvent3<int, int, int, 5>
{
public:
    PadDeviceChangedEvent()
        : UnidentifiedStaticEvent3<int, int, int, 5>("DeviceChanged", -1)
    {
    }

    virtual ~PadDeviceChangedEvent() { }
};

struct PlatPadManager
{
    PlatPadManager()
        : disableFreestyle(false)
        , disableClassic(false)
        , deviceChanged()
    {
    }

    void Initialize();
    void UpdateChannel(int channel);
    void UpdateDPD(int channel, unsigned int deviceType);
    WiiRemotePadStatus* GetRemoteStatus(int channel);
    WiiFreestylePadStatus* GetFreestyleStatus(int channel);
    WiiClassicPadStatus* GetClassicStatus(int channel);
    void SetDPDEnabled(int channel, bool enabled);
    bool IsDPDEnabled(int channel) const;

    PlatPadStatus status[WPAD_MAX_CONTROLLERS];
    bool connected[WPAD_MAX_CONTROLLERS];
    int type[WPAD_MAX_CONTROLLERS];
    bool disableFreestyle;
    bool disableClassic;
    bool dpdEnabled[WPAD_MAX_CONTROLLERS];
    bool dpdActive[WPAD_MAX_CONTROLLERS];
    bool dataFormatSet[WPAD_MAX_CONTROLLERS];
    unsigned char padding[2];
    PadDeviceChangedEvent deviceChanged;
};

#endif // NL_PLAT_PLATPADMANAGER_H
