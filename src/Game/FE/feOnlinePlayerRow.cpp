#include "Game/MiiManager.h"
#include "Game/FE/feOnlinePlayerRow.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinderFind_impl.h"
#include "Game/FE/feFinderDefault_impl.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

static const char* sOnlinePlayerSearchSlides[5] = { "SEARCHING", "CONNECTING", "INVITE", "ADD", "OFF" };
static const char* sOnlinePlayerStatusSlides[12] = {
    "WAITING FOR FRIEND", "WAITING_FOR_OPPONENT", "AVAILABLE", "BUSY", "CHOOSING_CAPTAIN", "CHOOSING_SIDEKICKS", "CAPTAINNAMES", "ESTABLISHING", "OFFLINE", "DECLINED", "INVITING", "OFFLINE"
};
static const char* sOnlinePlayerSideSlides[4] = { "HOME", "HOME", "AWAY", "GUEST" };

const char* GetOnlineCaptainSlideName(unsigned int captain)
{
    switch (captain)
    {
    case 0:
        return "MARIO";
    case 1:
        return "BOWSER";
    case 2:
        return "DAISY";
    case 3:
        return "DONKEYKONG";
    case 4:
        return "LUIGI";
    case 5:
        return "PEACH";
    case 6:
        return "WALUIGI";
    case 7:
        return "WARIO";
    case 8:
        return "YOSHI";
    case 9:
        return "BOWSERJR";
    case 10:
        return "DIDDYKONG";
    case 11:
        return "PETEY";
    default:
        return 0;
    }
}

void UpdateOnlinePlayerRow(FEOnlinePlayerRow* row, TLComponentInstance* instance,
    u16* name, int nameSize, u16* description, int descriptionSize, int index, bool value)
{
    TLInstance* off = FEFinder<TLInstance, 5>::FindOrDefault(instance, "off", "FRIEND_0");
    TLInstance* over = FEFinder<TLInstance, 5>::FindOrDefault(instance, "over", "FRIEND_0");
    off->SetVisible(row->mVisible);
    over->SetVisible(row->mVisible);

    FEFinder<TLInstance, 4>::FindOrDefault(off, "cancel")->SetVisible(row->mShowCancel);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "cancel")->SetVisible(row->mShowCancel);

    TLComponentInstance* searching = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "SEARCHING_ADD"));
    searching->SetActiveSlide(sOnlinePlayerSearchSlides[row->mSearchState], false, false);
    searching = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "SEARCHING_ADD"));
    searching->SetActiveSlide(sOnlinePlayerSearchSlides[row->mSearchState], false, false);

    TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::FindOrDefault(off, "NAME");
    nameText->SetString(row->mName);
    nameText->SetVisible(row->mSearchState == 4);
    nameText = FEFinder<TLTextInstance, 3>::FindOrDefault(over, "NAME");
    nameText->SetString(row->mName);
    nameText->SetVisible(row->mSearchState == 4);

    TLComponentInstance* status = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "STATUS"));
    status->SetActiveSlide(sOnlinePlayerStatusSlides[row->mStatus], false, false);
    status->SetVisible(row->mSearchState == 4);
    if (row->mStatus == 6)
    {
        TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(status, "NAMES");
        names->SetActiveSlide(GetOnlineCaptainSlideName(row->mCaptain), false, false);
    }
    status = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "STATUS"));
    status->SetActiveSlide(sOnlinePlayerStatusSlides[row->mStatus], false, false);
    status->SetVisible(row->mSearchState == 4);
    if (row->mStatus == 6)
    {
        TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(status, "NAMES");
        names->SetActiveSlide(GetOnlineCaptainSlideName(row->mCaptain), false, false);
    }
    bool show = row->mSearchState == 4
             && row->mStatus != 7 && row->mStatus != 9 && row->mStatus != 10;
    bool hasSide = row->mSide != 0;
    TLComponentInstance* guest = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "GUEST_HOME_AWAY"));
    guest->SetActiveSlide(sOnlinePlayerSideSlides[row->mSide], true, false);
    guest->SetVisible(show && hasSide);
    guest = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "GUEST_HOME_AWAY"));
    guest->SetActiveSlide(sOnlinePlayerSideSlides[row->mSide], true, false);
    guest->SetVisible(show && hasSide);

    FEFinder<TLInstance, 4>::FindOrDefault(off, "PLAYER CLASS")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "PLAYER CLASS")->SetVisible(false);
    WideString string = Format(WideString(g_pLocalization->GetString("ONLINE_RANKING")), row->mStats.mDisplayRank);
    nlStrNCpy(name, string.c_str(), nameSize);
    TLTextInstance* rank = FEFinder<TLTextInstance, 3>::FindOrDefault(off, "STATS", "Slide1", "RANK", "RANK");
    rank->SetString(name);
    rank->SetVisible(show && !row->mGuest);
    rank = FEFinder<TLTextInstance, 3>::FindOrDefault(over, "STATS", "Slide1", "RANK", "RANK");
    rank->SetString(name);
    rank->SetVisible(show && !row->mGuest);
    string = Format(WideString(g_pLocalization->GetString("ONLINE_SLOT_STATS")), row->mStats.mWins, row->mStats.mLosses, row->mStats.mScore);
    nlStrNCpy(description, string.c_str(), descriptionSize);
    TLTextInstance* record = FEFinder<TLTextInstance, 3>::FindOrDefault(off, "STATS", "Slide1", "RECORD", "RECORD");
    record->SetString(description);
    record->SetVisible(show && !row->mGuest);
    record = FEFinder<TLTextInstance, 3>::FindOrDefault(over, "STATS", "Slide1", "RECORD", "RECORD");
    record->SetString(description);
    record->SetVisible(show && !row->mGuest);

    FEFinder<TLImageInstance, 2>::FindOrDefault(off, "00_dummy_texture")->SetVisible(show);
    FEFinder<TLImageInstance, 2>::FindOrDefault(over, "00_dummy_texture")->SetVisible(show);

    TLImageInstance* background = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "Mii_btn", "Online_Mii_select_background");
    background->SetAssetVisible(show);
    background = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "Mii_btn", "Online_Mii_select_background");
    background->SetAssetVisible(show);
    bool valid = MiiManager::Instance()->CreateIcon((const RFLStoreData*)row->mMiiData, index, (RFLExpression)0);
    unsigned long texture = MiiManager::Instance()->GetIconTextureId(index);
    nlColour colour = { 0, 0, 0, 255 };
    TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(off, "Mii_btn", "Mii");
    image->GetTextureResource()->SetTextureHandle(texture);
    image->SetAssetVisible(valid && show && value);
    image->SetVisible(valid && show && value);
    if (row->mGuest)
        image->SetAssetColour(colour);
    image = FEFinder<TLImageInstance, 2>::Find(over, "Mii_btn", "Mii");
    image->GetTextureResource()->SetTextureHandle(texture);
    image->SetAssetVisible(valid && show && value);
    image->SetVisible(valid && show && value);
    if (row->mGuest)
        image->SetAssetColour(colour);

    TLImageInstance* shoulders = FEFinder<TLImageInstance, 2>::Find(off, "Mii_btn", "shoulders");
    shoulders->SetAssetVisible(false);
    shoulders->SetVisible(false);
    shoulders = FEFinder<TLImageInstance, 2>::Find(over, "Mii_btn", "shoulders");
    shoulders->SetAssetVisible(false);
    shoulders->SetVisible(false);
}

#include "Game/MiiManager.inl"
