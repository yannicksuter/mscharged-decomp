#ifndef GAME_FE_RESOURCE_MANAGER_H
#define GAME_FE_RESOURCE_MANAGER_H

#include "NL/nlBundleFile.h"
#include "NL/nlSingleton.h"
#include "NL/nlTask.h"
#include "types.h"

class FEScene;
class FESceneResource;
class FETextureResource;
class MemoryAllocator;

enum eFEResourceType
{
    FERT_UNKNOWN = -1,
    FERT_TEXTURE = 0,
    FERT_FONT = 1,
    FERT_SCENE = 2,
    FERT_RESOURCE_TYPE_COUNT = 3,
};

class FEResourceHandle
{
public:
    bool IsValid() const
    {
        return m_bValid;
    }

    eFEResourceType GetResourceType() const
    {
        return m_type;
    }

    unsigned long GetHashID() const
    {
        return m_hashID;
    }

    /* 0x00 */ FEResourceHandle* m_next;
    /* 0x04 */ FEResourceHandle* m_prev;
    /* 0x08 */ eFEResourceType m_type;
    /* 0x0C */ unsigned long m_hashID;
    /* 0x10 */ bool m_bValid;
    /* 0x11 */ u8 pad_0x11[3];
    /* 0x14 */ u32 field_0x14;
}; // size 0x18

enum ResourceResult
{
    FERR_WaitingForResource = 0,
    FERR_AlreadyLoaded = 1,
};

struct FEMiniBundle
{
    /* 0x00 */ FESceneResource* m_pSceneResource;
    /* 0x04 */ char m_szBundleFileName[32];
}; // size 0x24

struct PermanentBundleLoadState;

class FEResourceManager : public nlTask, public nlSingleton<FEResourceManager>
{
public:
    FEResourceManager();

    void Run(float dt)
    {
        Update(dt);
    }

    virtual const char* GetName()
    {
        return "FEResource Manager";
    }

    void Cleanup();
    void LoadPermanentResourceBundle(const char* szBundleFileName);
    void LoadPermanentResourceBundle(const char* szBundleFileName, void (*callback)());
    void LoadPermanentTextures();
    bool OpenOnDemandResourceBundle(const char* szBundleFileName, FileOpenAsyncCallback callback);
    void QueueResourceLoad(FEResourceHandle* pHandle, MemoryAllocator* pAllocator);
    void UnloadResource(FEResourceHandle* pFeResourceHandle);
    void UnloadPermanentResourceBundle();
    void Update(float dt);
    void fn_802FD26C(void* resourceInterface);
    void* fn_802FDD84();
    FEMiniBundle* fn_802FDD8C(const char* szBundleFileName);
    bool fn_802FDF3C(FEMiniBundle* miniBundle);

    static void TextureResourceLoadComplete(void* buffer, unsigned long uReadSize, unsigned long uParam);

private:
    ResourceResult IssueTextureLoadRequest(FETextureResource* pFeTextureResource, MemoryAllocator* pAllocator);
    ResourceResult IssueSceneContextSwitch(FESceneResource* pFeSceneResource);
    FEResourceHandle* FindExistingResourceInResourceList(FEResourceHandle* pFEResourceHandle);
    void RemoveResourceFromResourceList(FEResourceHandle* pFEResourceHandle);
    void AddResourceToResourceList(FEResourceHandle* pFEResourceHandle);
    static void fn_802FC850(void* buffer, unsigned long uReadSize, unsigned long uParam);
    static int fn_802FC858(PermanentBundleLoadState* state);
    static void fn_802FC9C4(void* buffer, unsigned long uReadSize, unsigned long uParam);

protected:
    /* 0x1D */ char m_szPermanentBundleFileName[32];
    /* 0x3D */ char m_szOnDemandBundleFileName[32];
    /* 0x5D */ bool m_bPermanentBundleLoadInProgress;
}; // size 0x60

#endif // GAME_FE_RESOURCE_MANAGER_H
