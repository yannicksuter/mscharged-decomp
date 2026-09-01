#include "Game/DB/SaveLoad.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/GameInfo.h"
#include "Game/ResetTask.h"
#include "Game/main.h"

#include <math.h>

#include "NL/MemAlloc.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlFile.h"
#include "NL/nlLocalization.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <string.h>
#include <wchar.h>

#define NAND_BANNER_SIZE(frames) (0x60A0 + (0x1200 * (frames)))
#define NAND_BANNER_ICON_SIZE    0x1200
#define NAND_BANNER_TEXTURE_SIZE 0x6000
#define NAND_BANNER_SET_ICON_SPEED(banner, frame, speed) \
    (banner)->iconSpeed = ((banner)->iconSpeed & ~(3 << ((frame) * 2))) | ((speed) << ((frame) * 2))

struct NANDBanner
{
    u32 magic;
    u32 flags;
    u16 iconSpeed;
    u8 reserved[0x16];
    wchar_t title[32];
    wchar_t subtitle[32];
    u8 bannerTexture[NAND_BANNER_TEXTURE_SIZE];
    u8 iconTexture[8][NAND_BANNER_ICON_SIZE];
};

struct TPLHeader
{
    u16 height;
    u16 width;
    u32 format;
    char* data;
};

struct TPLDescriptor
{
    TPLHeader* textureHeader;
    void* CLUTHeader;
};

struct TPLPalette
{
    u32 versionNumber;
    u32 numDescriptors;
    TPLDescriptor* descriptorArray;
};

extern "C" void TPLBind(TPLPalette* palette);

typedef void (*NANDResultCallback)(s32 result);

extern "C" s32 fn_80376934(s32 directory, NANDResultCallback callback);
extern "C" s32 fn_80376B08(const char* name, u8 permissions, NANDResultCallback callback);
extern "C" s32 fn_80376B68(const char* name, u8 permissions, NANDResultCallback callback);
extern "C" s32 fn_80376BC8(u32 blocks, u32 files, u32* answer, NANDResultCallback callback);
extern "C" s32 fn_80376C20(const char* name, NANDResultCallback callback);
extern "C" s32 fn_80376C78(const void* data, u32 size, NANDResultCallback callback);
extern "C" s32 fn_80376CF8(u32* size, NANDResultCallback callback);
extern "C" s32 fn_80376D6C(void** data, u32* size, NANDResultCallback callback, bool allocate);
extern "C" s32 fn_80376E3C(const char* name, s32 mode, NANDResultCallback callback);
extern "C" s32 fn_80376EB0(NANDResultCallback callback);
extern "C" bool fn_80376F18();

extern "C" bool fn_802C2C84(const char* path, bool create);
extern void nlPrintf(const char* format, ...);

extern BaseGameSceneManager* lbl_806E1838;
extern BaseGameSceneManager* lbl_806E1860;

static const char* SaveFileName = "Strikers2";
static const char* OnlineSaveFileName = "Online";
static const char* BannerFileName = "banner.bin";
bool SaveEnabled = true;
static bool RetryEnabled = true;

static TPLPalette* IconTPLs[4];

static u32 PendingIconLoads;
static void* SaveBuffer;
static NANDBanner* BannerBuffer;
static TPLPalette* RegionBannerTPL;
static bool IconDataLoaded;
static u32 CheckAnswer;
static u32 BannerFileLength;
static bool BannerFileExists;
static u32 BannerOpenMode;
static BaseGameSceneManager* SaveSceneManager;
bool SaveError;
static bool OnlineSaveLoaded;
bool NormalSaveLoaded;
bool InOperation;
static bool OnlineMode;

namespace SaveLoad
{
void CheckSaveSpace();
void CancelSave();
void ContinueWithoutSaving();
void BeginReset();
void DeleteSaveFile();
void OpenSaveForWriteCallback(s32 result);
void OpenSaveForReadCallback(s32 result);
void CheckSaveFileCallback(s32 result);
void CreateSaveFileCallback(s32 result);
void WriteSaveFileCallback(s32 result);
void ReadSaveFileCallback(s32 result);
void DeleteSaveFileCallback(s32 result);
void CloseCallback(s32 result);
void ContinueAfterCloseCallback(s32 result);
void StartSaveDirectoryCallback(s32 result);
void StartLoadDirectoryCallback(s32 result);
void CheckSaveAndBannerSpace();
void ChangeDirectoryCallback(s32 result);
void WriteSaveData();
bool ReadSaveData(u32 size);
void HandleNANDResult(s32 result);
void OpenBannerCallback(s32 result);
void BannerLengthCallback(s32 result);
void BannerFileLengthCallback(s32 result);
void DeleteInvalidBannerCallback(s32 result);
void CheckBannerSpaceCallback(s32 result);
void CreateBannerCallback(s32 result);
void WriteBannerCallback(s32 result);
void IconLoadedCallback(void* data, unsigned long size, void* userData);
void IconLoadsComplete();
void RegionBannerLoadedCallback(void* data, unsigned long size, void* userData);
void BannerWriteFinishedCallback(s32 result);
void FinishOperation(s32 result);
void BannerCloseCallback(s32 result);
void CheckSpaceAnswer(u32 answer, bool online);
void CheckNoCopyDirectoryCallback(s32 result);
void CreateNoCopyDirectoryCallback(s32 result);
void OpenBannerDirectoryCallback(s32 result);
} // namespace SaveLoad

static inline u32 Align32(u32 size)
{
    return (size + 0x1F) & ~0x1F;
}

static inline u32 SaveDataSize()
{
    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    if (OnlineMode)
    {
        return gameInfo->GetMemoryCardDataSize();
    }
    return (u32)gameInfo->GetUnknown806E0F90Block();
}

static inline const unsigned short* LocalizedString(const char* name)
{
    nlLocalization* localization = g_pLocalization;
    u32 hash = nlStringLowerHash(name);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* lookup = nlBSearch<nlLocalization::StringLookup, unsigned long>(
        hash, localization->m_LookupTable, localization->m_pFile->StringCount);
    if (lookup == 0)
    {
        return MissingLocString;
    }
    return localization->m_FirstString + lookup->StringOffset;
}

static inline FEPopupMenu* PushSavePopup()
{
    return (FEPopupMenu*)SaveSceneManager->Push((SceneList)10, SCREEN_NOTHING, false);
}

static inline void WriteLocalizedBanner(NANDResultCallback callback)
{
    const unsigned short* title = LocalizedString("SAVE_BANNER_TITLE");
    const unsigned short* comment = LocalizedString("SAVE_BANNER_COMMENT");

    memset(BannerBuffer->title, 0, sizeof(BannerBuffer->title));
    memset(BannerBuffer->subtitle, 0, sizeof(BannerBuffer->subtitle));
    memcpy(BannerBuffer->title, title, wcslen((const wchar_t*)title) * sizeof(unsigned short));
    memcpy(BannerBuffer->subtitle, comment, wcslen((const wchar_t*)comment) * sizeof(unsigned short));

    ResetTask::s_resetPaused = true;
    SaveLoad::HandleNANDResult(fn_80376C78(BannerBuffer, NAND_BANNER_SIZE(8), callback));
}

void SaveLoad::CheckSaveSpace()
{
    if (!SaveEnabled)
    {
        return;
    }

    u32 files = 1;
    u32 bannerBlocks = 0;
    if (!BannerFileExists)
    {
        files = 2;
        bannerBlocks = NAND_BANNER_SIZE(8);
        bannerBlocks = (u32)(float)ceil((float)bannerBlocks / 16384.0f);
    }

    u32 saveBlocks;
    if (OnlineMode)
    {
        saveBlocks = GameInfoManager::GetInstance()->GetMemoryCardDataSize() + sizeof(SaveFileHeader);
    }
    else
    {
        saveBlocks = (u32)GameInfoManager::GetInstance()->GetUnknown806E0F90Block() + sizeof(SaveFileHeader);
    }
    saveBlocks = Align32(saveBlocks);
    saveBlocks = (u32)(float)ceil((float)saveBlocks / 16384.0f);
    HandleNANDResult(fn_80376BC8(saveBlocks + bannerBlocks, files, &CheckAnswer, CheckSaveFileCallback));

    if (OnlineMode)
    {
        OnlineSaveLoaded = true;
    }
    else
    {
        NormalSaveLoaded = true;
    }
}

void SaveLoad::CancelSave()
{
    ResetTask::s_resetPaused = false;
    SaveEnabled = false;
    InOperation = false;
}

void SaveLoad::ContinueWithoutSaving()
{
    struct SceneState
    {
        u8 unused[0x684];
        bool enabled;
    };

    SceneState* scene = (SceneState*)lbl_806E1838->GetScene((SceneList)1);
    if (scene != 0)
    {
        scene->enabled = false;
    }
    ResetTask::s_resetPaused = false;
    OnlineSaveLoaded = false;
    InOperation = false;
}

void SaveLoad::BeginReset()
{
    ResetTask::s_ResetMode = 3;
    if (ResetTask::s_ResetState == RS_RUNNING)
    {
        ResetTask::s_ResetState = RS_STARTRESET;
    }
}

void SaveLoad::DeleteSaveFile()
{
    const char* filename = OnlineMode ? OnlineSaveFileName : SaveFileName;
    s32 result = fn_80376C20(filename, DeleteSaveFileCallback);
    if (result != 0)
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::OpenSaveForWriteCallback(s32 result)
{
    if (result == -12)
    {
        if (RetryEnabled && !OnlineMode)
        {
            FEPopupMenu* popup = PushSavePopup();
            Function<FnVoidVoid> save(CheckSaveSpace);
            Function<FnVoidVoid> cancel(CancelSave);
            popup->Create((ePopupMenu)0x3D, save, cancel);
            RetryEnabled = false;
        }
        else
        {
            CheckSaveSpace();
        }
    }
    else if (result == 0)
    {
        WriteSaveData();
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::OpenSaveForReadCallback(s32 result)
{
    if (result == 0)
    {
        SaveBuffer = 0;
        u32 size;
        if (OnlineMode)
        {
            size = GameInfoManager::GetInstance()->GetMemoryCardDataSize() + sizeof(SaveFileHeader);
        }
        else
        {
            size = (u32)GameInfoManager::GetInstance()->GetUnknown806E0F90Block() + sizeof(SaveFileHeader);
        }
        size = Align32(size);
        SaveBuffer = nlMalloc(size, 0x20, true);
        HandleNANDResult(fn_80376D6C(&SaveBuffer, &size, ReadSaveFileCallback, true));
        return;
    }

    if (result == -12)
    {
        if (!OnlineMode)
        {
            FEPopupMenu* popup = PushSavePopup();
            popup->Create(
                (ePopupMenu)0x3E,
                Function<FnVoidVoid>(CheckSaveSpace),
                Function<FnVoidVoid>(CancelSave));
            RetryEnabled = false;
        }
        else if (OnlineMode && RetryEnabled)
        {
            FEPopupMenu* popup = PushSavePopup();
            popup->Create((ePopupMenu)0x47, Function<FnVoidVoid>(CheckSaveSpace));
            RetryEnabled = false;
        }
        else
        {
            CheckSaveSpace();
        }
        return;
    }
    HandleNANDResult(result);
}

void SaveLoad::CheckSaveFileCallback(s32 result)
{
    if (result == 0)
    {
        if (CheckAnswer == 0)
        {
            ResetTask::s_resetPaused = true;
            const char* filename = OnlineMode ? OnlineSaveFileName : SaveFileName;
            HandleNANDResult(fn_80376B68(filename, 0x30, CreateSaveFileCallback));
        }
        else
        {
            CheckSpaceAnswer(CheckAnswer, OnlineMode);
        }
    }
    else
    {
        HandleNANDResult(result);
    }
    CheckAnswer = 0;
}

void SaveLoad::CreateSaveFileCallback(s32 result)
{
    if (result == -6 || result == 0)
    {
        const char* filename = OnlineMode ? OnlineSaveFileName : SaveFileName;
        HandleNANDResult(fn_80376E3C(filename, 2, OpenSaveForWriteCallback));
    }
    else
    {
        ResetTask::s_resetPaused = false;
        HandleNANDResult(result);
    }
}

void SaveLoad::WriteSaveFileCallback(s32 result)
{
    nlFree(SaveBuffer);
    SaveBuffer = 0;

    u32 expectedSize = Align32(SaveDataSize() + sizeof(SaveFileHeader));
    if ((u32)result == expectedSize)
    {
        HandleNANDResult(fn_80376EB0(ContinueAfterCloseCallback));
    }
    else
    {
        fn_80376EB0(CloseCallback);
        HandleNANDResult(result);
    }
    ResetTask::s_resetPaused = false;
}

void SaveLoad::ReadSaveFileCallback(s32 result)
{
    u32 expectedSize = Align32(SaveDataSize() + sizeof(SaveFileHeader));
    if ((u32)result == expectedSize)
    {
        bool valid = ReadSaveData(result);
        if (valid && !OnlineMode)
        {
            GameInfoManager::GetInstance()->GetUnknown0xA0();
        }
        fn_80376EB0(valid ? ContinueAfterCloseCallback : CloseCallback);
        if (OnlineMode)
        {
            OnlineSaveLoaded = true;
        }
        else
        {
            NormalSaveLoaded = true;
        }
        HandleNANDResult(0);
    }
    else if (result >= 0)
    {
        nlFree(SaveBuffer);
        SaveBuffer = 0;
        nlPrintf("SAVE FILE IS CORRUPT: The size is incorrect.\n");
        ResetTask::s_resetPaused = false;
        FEPopupMenu* popup = PushSavePopup();
        Function<FnVoidVoid> remove(DeleteSaveFile);
        popup->Create((ePopupMenu)0x45, remove);
        fn_80376EB0(CloseCallback);
        HandleNANDResult(0);
    }
    else
    {
        if (OnlineMode)
        {
            OnlineSaveLoaded = true;
        }
        else
        {
            NormalSaveLoaded = true;
        }
        fn_80376EB0(CloseCallback);
        HandleNANDResult(result);
    }
}

void SaveLoad::DeleteSaveFileCallback(s32 result)
{
    if (result == 0)
    {
        FEPopupMenu* popup = PushSavePopup();
        if (OnlineMode)
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> continueWithoutSaving(ContinueWithoutSaving);
            popup->Create((ePopupMenu)0x40, reset, continueWithoutSaving);
        }
        else
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> cancel(CancelSave);
            popup->Create((ePopupMenu)0x3E, reset, cancel);
        }
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::CloseCallback(s32)
{
}

void SaveLoad::ContinueAfterCloseCallback(s32)
{
    if (OnlineMode)
    {
        HandleNANDResult(fn_80376934(0, OpenBannerDirectoryCallback));
    }
    else
    {
        HandleNANDResult(fn_80376E3C(BannerFileName, 2, OpenBannerCallback));
    }
}

void SaveLoad::StartSaveDirectoryCallback(s32 result)
{
    if (result == 0)
    {
        BannerOpenMode = 1;
        InOperation = true;
        HandleNANDResult(fn_80376E3C(BannerFileName, 1, BannerLengthCallback));
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::StartLoadDirectoryCallback(s32 result)
{
    if (result == 0)
    {
        BannerOpenMode = 0;
        InOperation = true;
        HandleNANDResult(fn_80376E3C(BannerFileName, 1, BannerLengthCallback));
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::CheckSaveAndBannerSpace()
{
    u32 files = 2;
    u32 bannerBlocks = 0;
    if (!BannerFileExists)
    {
        files = 3;
        bannerBlocks = (u32)ceil((float)NAND_BANNER_SIZE(8) / 16384.0f);
    }
    u32 saveBlocks = (u32)ceil((float)Align32(
                                   GameInfoManager::GetInstance()->GetMemoryCardDataSize() + sizeof(SaveFileHeader))
                               / 16384.0f);
    HandleNANDResult(fn_80376BC8(saveBlocks + bannerBlocks, files, &CheckAnswer, CheckNoCopyDirectoryCallback));
}

void SaveLoad::ChangeDirectoryCallback(s32 result)
{
    if (result == -12)
    {
        FEPopupMenu* popup = PushSavePopup();
        Function<FnVoidVoid> retry(CheckSaveAndBannerSpace);
        popup->Create((ePopupMenu)0x47, retry);
        RetryEnabled = false;
    }
    else if (result == 0)
    {
        const char* filename = OnlineSaveFileName;
        if (BannerOpenMode == 0)
        {
            HandleNANDResult(fn_80376E3C(filename, 1, OpenSaveForReadCallback));
        }
        else
        {
            HandleNANDResult(fn_80376E3C(filename, 2, OpenSaveForWriteCallback));
        }
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::StartSave(bool online)
{
    if (fn_802C2C84("/user/no_save", false))
    {
        SaveEnabled = false;
        return;
    }

    OnlineMode = online;
    SaveSceneManager = lbl_806E1838 != 0 ? lbl_806E1838 : lbl_806E1860;
    if (SaveEnabled && !fn_80376F18())
    {
        BannerFileExists = false;
        HandleNANDResult(fn_80376934(0, StartSaveDirectoryCallback));
    }
}

void SaveLoad::StartLoad(bool online)
{
    if (fn_802C2C84("/user/no_save", false))
    {
        SaveEnabled = false;
        return;
    }

    OnlineMode = online;
    bool loaded = online ? OnlineSaveLoaded : NormalSaveLoaded;
    SaveSceneManager = lbl_806E1838 != 0 ? lbl_806E1838 : lbl_806E1860;
    if (SaveEnabled && !loaded && !fn_80376F18())
    {
        BannerFileExists = false;
        RetryEnabled = true;
        HandleNANDResult(fn_80376934(0, StartLoadDirectoryCallback));
    }
}

u32 SaveLoad::GetSaveBlockSize()
{
    u32 size;
    if (OnlineMode)
    {
        size = GameInfoManager::GetInstance()->GetMemoryCardDataSize() + sizeof(SaveFileHeader);
    }
    else
    {
        size = (u32)GameInfoManager::GetInstance()->GetUnknown806E0F90Block() + sizeof(SaveFileHeader);
    }
    size = Align32(size);
    return Align32(size + NAND_BANNER_SIZE(8));
}

void SaveLoad::WriteSaveData()
{
    ResetTask::s_resetPaused = true;
    u32 dataSize = SaveDataSize() + sizeof(SaveFileHeader);
    u32 alignedSize = Align32(dataSize);
    SaveBuffer = nlMalloc(alignedSize, 0x20, true);

    GameInfoManager* gameInfo = GameInfoManager::GetInstance();
    gameInfo->unknown_0x9C = (void*)nlRandom(0xFFFFFFFF, &nlDefaultSeed);
    if (OnlineMode)
    {
        gameInfo->GetMemoryCardData((u8*)SaveBuffer + sizeof(SaveFileHeader));
    }
    else
    {
        gameInfo->SerializeSettings((u8*)SaveBuffer + sizeof(SaveFileHeader));
    }

    SaveFileHeader* header = (SaveFileHeader*)SaveBuffer;
    header->Size = dataSize;
    header->CRC = nlChecksum32((u8*)SaveBuffer + sizeof(SaveFileHeader), alignedSize - sizeof(SaveFileHeader));
    HandleNANDResult(fn_80376C78(SaveBuffer, alignedSize, WriteSaveFileCallback));
}

bool SaveLoad::ReadSaveData(u32 size)
{
    SaveFileHeader* header = (SaveFileHeader*)SaveBuffer;
    u32 checksum = nlChecksum32((u8*)SaveBuffer + sizeof(SaveFileHeader), size - sizeof(SaveFileHeader));
    if (header->CRC != checksum)
    {
        nlPrintf("SAVE FILE IS CORRUPT: The checksum is incorrect.\n");
        nlFree(SaveBuffer);
        SaveBuffer = 0;
        ResetTask::s_resetPaused = false;
        FEPopupMenu* popup = PushSavePopup();
        Function<FnVoidVoid> remove(DeleteSaveFile);
        popup->Create((ePopupMenu)0x45, remove);
        return false;
    }

    void* allocation = SaveBuffer;
    void* data = (u8*)SaveBuffer + sizeof(SaveFileHeader);
    SaveBuffer = data;
    if (OnlineMode)
    {
        GameInfoManager::GetInstance()->SetMemoryCardData(data);
    }
    else
    {
        GameInfoManager::GetInstance()->DeserializeSettings(data);
    }
    nlFree(allocation);
    SaveBuffer = 0;
    return true;
}

void SaveLoad::HandleNANDResult(s32 result)
{
    if (result == 0)
    {
        return;
    }

    ResetTask::s_resetPaused = false;
    SaveError = true;
    if (result == -5 || result == -15)
    {
        FEPopupMenu* popup = PushSavePopup();
        Function<FnVoidVoid> retry(CheckSaveSpace);
        Function<FnVoidVoid> cancel(CancelSave);
        Function<FnVoidVoid> reset(BeginReset);
        popup->Create((ePopupMenu)0x43, retry, cancel, reset);
    }
    else if (result == -6 || result == -12)
    {
        FEPopupMenu* popup = PushSavePopup();
        Function<FnVoidVoid> retry(CheckSaveSpace);
        Function<FnVoidVoid> cancel(CancelSave);
        popup->Create((ePopupMenu)0x44, retry, cancel);
    }
    else
    {
        InOperation = false;
    }
}

void SaveLoad::OpenBannerCallback(s32 result)
{
    if (BannerFileExists && (result == 0 || result == -6))
    {
        WriteLocalizedBanner(BannerWriteFinishedCallback);
    }
    else
    {
        u32 blocks = (u32)ceil((float)NAND_BANNER_SIZE(8) / 16384.0f);
        HandleNANDResult(fn_80376BC8(blocks, 1, &CheckAnswer, CheckBannerSpaceCallback));
    }
}

void SaveLoad::BannerLengthCallback(s32 result)
{
    if (result == 0 || result == -6)
    {
        BannerFileLength = 0;
        HandleNANDResult(fn_80376CF8(&BannerFileLength, BannerFileLengthCallback));
    }
    else
    {
        BannerFileExists = false;
        if (OnlineMode)
        {
            result = fn_80376934(1, ChangeDirectoryCallback);
        }
        else if (BannerOpenMode == 0)
        {
            result = fn_80376E3C(SaveFileName, 1, OpenSaveForReadCallback);
        }
        else
        {
            result = fn_80376E3C(SaveFileName, 2, OpenSaveForWriteCallback);
        }
        HandleNANDResult(result);
    }
}

void SaveLoad::BannerFileLengthCallback(s32 result)
{
    if (result == 0)
    {
        if (BannerFileLength == NAND_BANNER_SIZE(8))
        {
            BannerFileExists = true;
            HandleNANDResult(fn_80376EB0(BannerCloseCallback));
        }
        else
        {
            BannerFileExists = false;
            HandleNANDResult(fn_80376EB0(DeleteInvalidBannerCallback));
        }
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::DeleteInvalidBannerCallback(s32 result)
{
    if (result == 0)
    {
        fn_80376C20(BannerFileName, BannerCloseCallback);
    }
    HandleNANDResult(result);
}

void SaveLoad::CheckBannerSpaceCallback(s32 result)
{
    if (result == 0)
    {
        if (CheckAnswer == 0)
        {
            ResetTask::s_resetPaused = true;
            HandleNANDResult(fn_80376B68(BannerFileName, 0x30, CreateBannerCallback));
        }
        else
        {
            CheckSpaceAnswer(CheckAnswer, OnlineMode);
        }
    }
    else
    {
        HandleNANDResult(result);
    }
    CheckAnswer = 0;
}

void SaveLoad::CreateBannerCallback(s32 result)
{
    if (result == -6 || result == 0)
    {
        HandleNANDResult(fn_80376E3C(BannerFileName, 2, WriteBannerCallback));
    }
    else
    {
        ResetTask::s_resetPaused = false;
        HandleNANDResult(result);
    }
}

void SaveLoad::WriteBannerCallback(s32 result)
{
    if (result != 0)
    {
        HandleNANDResult(result);
        return;
    }
    WriteLocalizedBanner(BannerWriteFinishedCallback);
}

void LoadMemoryCardIconData()
{
    if (IconDataLoaded)
    {
        return;
    }

    BannerBuffer->magic = 0x5749424E;
    BannerBuffer->flags = 0;
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 0, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 1, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 2, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 3, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 4, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 5, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 6, 2);
    NAND_BANNER_SET_ICON_SPEED(BannerBuffer, 7, 2);

    memset(BannerBuffer->title, 0, sizeof(BannerBuffer->title) + sizeof(BannerBuffer->subtitle));
    memset(BannerBuffer->bannerTexture, 0, sizeof(BannerBuffer->bannerTexture));
    memset(BannerBuffer->iconTexture, 0, sizeof(BannerBuffer->iconTexture));

    const unsigned short* title = LocalizedString("SAVE_BANNER_TITLE");
    const unsigned short* comment = LocalizedString("SAVE_BANNER_COMMENT");
    memcpy(BannerBuffer->title, title, wcslen((const wchar_t*)title) * sizeof(unsigned short));
    memcpy(BannerBuffer->subtitle, comment, wcslen((const wchar_t*)comment) * sizeof(unsigned short));

    nlLoadEntireFileAsync("/art/fe/icon.tpl", SaveLoad::IconLoadedCallback, &IconTPLs[3], 0x20, AllocateStart, 0, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("/art/fe/icon_2.tpl", SaveLoad::IconLoadedCallback, &IconTPLs[2], 0x20, AllocateStart, 0, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("/art/fe/icon_3.tpl", SaveLoad::IconLoadedCallback, &IconTPLs[1], 0x20, AllocateStart, 0, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("/art/fe/icon_4.tpl", SaveLoad::IconLoadedCallback, &IconTPLs[0], 0x20, AllocateStart, 0, 0, &VirtualAllocator);
    PendingIconLoads = 4;
}

void SaveLoad::IconLoadedCallback(void* data, unsigned long, void* userData)
{
    *(TPLPalette**)userData = (TPLPalette*)data;
    --PendingIconLoads;
    if (PendingIconLoads == 0)
    {
        PendingIconLoads = 0;
        IconLoadsComplete();
    }
}

void SaveLoad::IconLoadsComplete()
{
    for (s32 i = 0; i < 4; ++i)
    {
        TPLBind(IconTPLs[i]);
    }

    const char* region;
    switch (GetRegion())
    {
    case 1:
        region = "EU";
        break;
    case 2:
        region = "JP";
        break;
    default:
        region = "US";
        break;
    }

    char path[0x20];
    nlSNPrintf(path, sizeof(path), "art/fe/%s_banner.tpl", region);
    nlLoadEntireFileAsync(path, RegionBannerLoadedCallback, &RegionBannerTPL, 0x20, AllocateStart, 0, 0, &VirtualAllocator);
}

void SaveLoad::RegionBannerLoadedCallback(void* data, unsigned long, void* userData)
{
    *(TPLPalette**)userData = (TPLPalette*)data;
    TPLBind(RegionBannerTPL);

    memcpy(BannerBuffer->iconTexture[0], IconTPLs[0]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[1], IconTPLs[0]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[2], IconTPLs[0]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[3], IconTPLs[1]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[4], IconTPLs[2]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[5], IconTPLs[3]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[6], IconTPLs[2]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->iconTexture[7], IconTPLs[1]->descriptorArray[0].textureHeader->data, NAND_BANNER_ICON_SIZE);
    memcpy(BannerBuffer->bannerTexture, RegionBannerTPL->descriptorArray[0].textureHeader->data, NAND_BANNER_TEXTURE_SIZE);
    IconDataLoaded = true;

    for (s32 i = 0; i < 4; ++i)
    {
        ::operator delete[](IconTPLs[i]);
        IconTPLs[i] = 0;
    }
    ::operator delete[](RegionBannerTPL);
    RegionBannerTPL = 0;
}

void SaveLoad::BannerWriteFinishedCallback(s32)
{
    ResetTask::s_resetPaused = false;
    HandleNANDResult(fn_80376EB0(FinishOperation));
}

void SaveLoad::FinishOperation(s32)
{
    InOperation = false;
}

void SaveLoad::BannerCloseCallback(s32 result)
{
    if (result == 0)
    {
        if (OnlineMode)
        {
            result = fn_80376934(1, ChangeDirectoryCallback);
        }
        else if (BannerOpenMode == 0)
        {
            result = fn_80376E3C(SaveFileName, 1, OpenSaveForReadCallback);
        }
        else
        {
            result = fn_80376E3C(SaveFileName, 2, OpenSaveForWriteCallback);
        }
    }
    HandleNANDResult(result);
}

void SaveLoad::AllocateBannerBuffer()
{
    BannerBuffer = (NANDBanner*)nlMalloc(NAND_BANNER_SIZE(8), 0x20, false);
}

void SaveLoad::FreeBannerBuffer()
{
    ::operator delete(BannerBuffer);
    BannerBuffer = 0;
}

void SaveLoad::CheckSpaceAnswer(u32 answer, bool online)
{
    ResetTask::s_resetPaused = false;
    FEPopupMenu* popup = PushSavePopup();
    if ((answer & 1) != 0 || (answer & 4) != 0)
    {
        if (online)
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> continueWithoutSaving(ContinueWithoutSaving);
            popup->Create((ePopupMenu)0x40, reset, continueWithoutSaving);
        }
        else
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> cancel(CancelSave);
            popup->Create((ePopupMenu)0x3F, reset, cancel);
        }
    }
    else if ((answer & 2) != 0 || (answer & 8) != 0)
    {
        if (online)
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> continueWithoutSaving(ContinueWithoutSaving);
            popup->Create((ePopupMenu)0x42, reset, continueWithoutSaving);
        }
        else
        {
            Function<FnVoidVoid> reset(BeginReset);
            Function<FnVoidVoid> cancel(CancelSave);
            popup->Create((ePopupMenu)0x41, reset, cancel);
        }
    }
    else
    {
        HandleNANDResult(-64);
    }
}

void SaveLoad::CheckNoCopyDirectoryCallback(s32 result)
{
    if (result == 0)
    {
        if (CheckAnswer == 0)
        {
            HandleNANDResult(fn_80376B08("nocopy", 0x30, CreateNoCopyDirectoryCallback));
        }
        else
        {
            CheckSpaceAnswer(CheckAnswer, OnlineMode);
        }
    }
    else
    {
        HandleNANDResult(result);
    }
    CheckAnswer = 0;
}

void SaveLoad::CreateNoCopyDirectoryCallback(s32 result)
{
    if (result == 0)
    {
        HandleNANDResult(fn_80376934(1, ChangeDirectoryCallback));
    }
    else
    {
        HandleNANDResult(result);
    }
}

void SaveLoad::OpenBannerDirectoryCallback(s32 result)
{
    if (result == 0)
    {
        HandleNANDResult(fn_80376E3C(BannerFileName, 2, OpenBannerCallback));
    }
    else
    {
        HandleNANDResult(result);
    }
}
