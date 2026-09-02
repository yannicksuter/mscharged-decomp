#ifndef GAME_RESOURCE_INTERFACE_802CC094_H
#define GAME_RESOURCE_INTERFACE_802CC094_H

class ResourceInterface_802CC094
{
public:
    virtual void fn_08();
    virtual unsigned long MarkResource();
    virtual void ReleaseResource(unsigned long marker);
    virtual unsigned long GetFreeMemory();
};

#endif // GAME_RESOURCE_INTERFACE_802CC094_H
