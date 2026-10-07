#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/SH/SHOnlineMiiSelectOverlay.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/main.h"
#include "Game/PadActions.h"
#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "NL/gl/glMemory.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/MemAlloc.h"
#include "NL/nlPrint.h"
#include "Game/SH/SHTitleScreen.h"
#include "Game/SH/SHMainMenu.h"
#include "Game/SH/SHChooseCaptains.h"
#include "Game/SH/SHChooseSidekicks.h"
#include "Game/SH/SHChooseSides.h"
#include "Game/SH/SHStadiumSelect.h"
#include "Game/SH/SHCupCheater.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/SH/SHOptions.h"
#include "Game/FE/feOptionsSubMenus.h"
#include "Game/FE/SHCrossFader.h"
#include "Game/SH/SHLoading.h"
#include "Game/SH/SHBootLoading.h"
#include "Game/SH/SHMoviePlayer.h"
#include "Game/SH/SHCredits.h"
#include "Game/SH/SHNetworkStart.h"
#include "Game/SH/SHGameplayOptions.h"
#include "Game/SH/SHOptionsCheatsList.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHRoadToStrikersCupHub.h"
#include "Game/SH/SHCupHub.h"
#include "Game/SH/SHGameResults.h"
#include "Game/SH/SHCupKnockout.h"
#include "Game/SH/SHCupFinalRounds.h"
#include "Game/SH/SHStrikerCupStandings.h"
#include "Game/SH/SHStrikerCupAwards.h"
#include "Game/SH/SHCupNews.h"
#include "Game/SH/SHOnlineHub.h"
#include "Game/SH/SHOnlinePlayerCount.h"
#include "Game/SH/SHOnlineGuestControllerSelect.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/SH/SHOnlineInvitePreview.h"
#include "Game/SH/SHOnlineRanking.h"
#include "Game/SH/SHOnlineFriends.h"
#include "Game/SH/SHOnlineFriendCodeEntry.h"
#include "Game/SH/SHOnlineMatchmakingDraft.h"
#include "Game/SH/SHOnlineFriendsDraft.h"
#include "Game/SH/SHOnlineLogin.h"
#include "Game/SH/SHOnlineInviteResponse.h"
#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/SH/SHOnlineMiiSelect.h"
#include "Game/SH/SHOnlineFriendsChooseSides.h"
#include "Game/SH/SHOnlineConnectionQuality.h"
#include "Game/SH/SHHallOfFameRoom.h"
#include "Game/SH/SHHallOfFameSummary.h"
#include "Game/SH/SHHallOfFamePlayerCard.h"
#include "Game/SH/SHHallOfFameHistory.h"
#include "Game/SH/SHChallengeSelect.h"
#include "Game/SH/SHStrikerTimesChallenge.h"
#include "Game/SH/SHPause.h"
#include "Game/OverlayHandlerHUD.h"
#include "Game/OverlayHandlerInGameText.h"
#include "Game/SH/SHPausePostGame.h"
#include "Game/FE/OnlineRanking.h"
#include "Game/OverlayHandlerStrikerTimes.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/OverlayHandlerDemo.h"
#include "Game/BaseSceneHandler.h"
#include "Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.h"
#include "Game/FE/Overlay/OverlayHandlerPIP.h"
#include "Game/FE/Overlay/OverlayHandlerSuperAbility.h"
#include "Game/FE/Overlay/OverlayHandlerChallengePreview.h"
#include "Game/FE/Overlay/OverlayHandlerControllerMap.h"
#include "Game/FE/Overlay/OverlayHandlerDefensivePlay.h"

FEMiniBundle* gFEMiniBundle;
GLResourcePool* gFEResourcePool;
unsigned long gFEResourceMarker;

void CreateFEResourcePool()
{
    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        GLMemoryRequirement requirements[2] = {
            { GLM_Header, 0x5000 },
            { GLM_TextureData, 0x2CCCCC },
        };
        gFEResourcePool
            = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    }
    else
    {
        GLMemoryRequirement requirements[2] = {
            { GLM_Header, 0x5000 },
            { GLM_TextureData, 0x200000 },
        };
        gFEResourcePool
            = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    }

    gFEResourceMarker = gFEResourcePool->MarkResource();
    FEResourceManager::Instance()->SetResourcePool(gFEResourcePool);
}

void CreateLargeFEResourcePool()
{
    GLMemoryRequirement requirements[2] = {
        { GLM_Header, 0xC800 },
        { GLM_TextureData, 0xA00000 },
    };
    gFEResourcePool
        = glCreateResourcePool(requirements, 2, "FEResourceManagerPool");
    gFEResourceMarker = gFEResourcePool->MarkResource();
    FEResourceManager::Instance()->SetResourcePool(gFEResourcePool);
}

void DestroyFEResourcePool()
{
    if (gFEResourcePool != 0)
    {
        gFEResourcePool->ReleaseResource(gFEResourceMarker);
        FEResourceManager::Instance()->SetResourcePool(0);
        glDestroyResourcePool(gFEResourcePool);
        gFEResourcePool = 0;
    }
}

void LoadFEMiniBundle(const char* bundleFileName)
{
    gFEMiniBundle
        = FEResourceManager::Instance()->LoadMiniBundle(bundleFileName);
}

bool UnloadFEMiniBundle()
{
    return FEResourceManager::Instance()->UnloadMiniBundle(gFEMiniBundle);
}

GLResourcePool* GetFEResourcePool()
{
    return gFEResourcePool;
}

SceneEntry SceneEntryTable[] = {
    { SCENE_TITLE, "art/fe/sms2_start.fen" },
    { SCENE_MAIN_MENU, "art/fe/main_menu_v3.fen" },
    { SCENE_CHOOSE_CAPTAINS_DOMINATION, "art/fe/choose_captains_domination.fen" },
    { SCENE_CHOOSE_SIDEKICKS_DOMINATION, "art/fe/choose_sidekicks_domination.fen" },
    { (SceneList)4, "art/fe/domination_choose_sides.fen" },
    { (SceneList)5, "art/fe/choose_stadiums_domination.fen" },
    { SCENE_CHOOSE_CAPTAINS_STRIKER_CUP, "art/fe/choose_captains_domination.fen" },
    { SCENE_CHOOSE_SIDEKICKS_STRIKER_CUP, "art/fe/choose_sidekicks_domination.fen" },
    { (SceneList)8, "art/fe/domination_choose_sides.fen" },
    { (SceneList)9, "art/fe/cup_cheater.fen" },
    { SCENE_POPUP_MENU, "art/fe/popup_menu.fen" },
    { (SceneList)11, "art/fe/spoils_menu_v2.fen" },
    { (SceneList)12, "art/fe/custom_tournament_options_v2.fen" },
    { (SceneList)13, "art/fe/options_main_menu.fen" },
    { SCENE_AUDIO_OPTIONS, "art/fe/options_audio_options.fen" },
    { SCENE_VISUAL_OPTIONS, "art/fe/options_visual_options.fen" },
    { (SceneList)16, "art/fe/englegal.fen" },
    { SCENE_SUPER_LOADING, "art/fe/loadingscreen.fen" },
    { SCENE_BOOT_LOADING, "art/fe/boot_loading.fen" },
    { SCENE_BOOT_LOADING_JPN, "art/fe/boot_loading_jpn.fen" },
    { (SceneList)20, "art/fe/movieplayer.fen" },
    { (SceneList)21, "art/fe/movieplayer.fen" },
    { SCENE_INTRO_MOVIE, "art/fe/movieplayer.fen" },
    { SCENE_CREDITS, "art/fe/credits.fen" },
    { (SceneList)24, "art/fe/sms2_network_start.fen" },
    { SCENE_ASYNC_LOADING, "art/fe/asyncloading.fen" },
    { SCENE_WIDESCREEN_LOADING, "art/fe/WIDESCREEN_LOADING_SCREEN.fen" },
    { SCENE_GAMEPLAY_OPTIONS, "art/fe/options.fen" },
    { SCENE_OPTIONS_CHEATS_LIST, "art/fe/options_cheats_list.fen" },
    { (SceneList)29, "art/fe/fe_overlay.fen" },
    { (SceneList)30, "art/fe/wiicursor.fen" },
    { (SceneList)31, "art/fe/roadtostrikerscup_hub.fen" },
    { (SceneList)32, "art/fe/cup_schedule.fen" },
    { (SceneList)33, "art/fe/striker_times_2.fen" },
    { (SceneList)34, "art/fe/cup_knockout.fen" },
    { (SceneList)35, "art/fe/cup_final_rounds.fen" },
    { (SceneList)36, "art/fe/striker_cup_standings.fen" },
    { (SceneList)37, "art/fe/striker_cup_awards_golden_boot.fen" },
    { (SceneList)38, "art/fe/striker_cup_awards_brick_wall.fen" },
    { (SceneList)39, "art/fe/striker_times_2.fen" },
    { SCENE_ONLINE_MENU, "art/fe/online_menu.fen" },
    { (SceneList)41, "art/fe/online_ranked_menu.fen" },
    { (SceneList)42, "art/fe/online_unranked_menu.fen" },
    { SCENE_ONLINE_GUEST_CONTROLLER_SELECT, "art/fe/online_active_controllers.fen" },
    { (SceneList)44, "art/fe/online_invite_players.fen" },
    { SCENE_ONLINE_INVITE_PREVIEW, "art/fe/online_preview.fen" },
    { SCENE_ONLINE_RANKING, "art/fe/leaderboard_rankings.fen" },
    { (SceneList)47, "art/fe/online_friends_list.fen" },
    { SCENE_ONLINE_FRIEND_CODE_ENTRY, "art/fe/online_friends_code_enter.fen" },
    { (SceneList)49, "art/fe/online_draft_screen.fen" },
    { (SceneList)50, "art/fe/online_draft_screen.fen" },
    { SCENE_ONLINE_LOGIN, "art/fe/online_login.fen" },
    { SCENE_ONLINE_INVITE_RESPONSE, "art/fe/online_invitation.fen" },
    { SCENE_ONLINE_INVITE_STATUS, "art/fe/online_login.fen" },
    { SCENE_ONLINE_MII_SELECT, "art/fe/online_mii_select.fen" },
    { SCENE_ONLINE_MII_SELECT_OVERLAY, "art/fe/online_mii_select_overlay.fen" },
    { (SceneList)56, "art/fe/online_friends_choose_sides.fen" },
    { (SceneList)57, "art/fe/online_connection_quality.fen" },
    { (SceneList)58, "art/fe/hof_profile.fen" },
    { (SceneList)59, "art/fe/hof_game_summary.fen" },
    { (SceneList)60, "art/fe/hof_game_summary.fen" },
    { (SceneList)61, "art/fe/hof_game_summary.fen" },
    { (SceneList)62, "art/fe/hof_fire_cup.fen" },
    { (SceneList)63, "art/fe/hof_striker_cup.fen" },
    { (SceneList)64, "art/fe/hof_crystal_cup.fen" },
    { (SceneList)65, "art/fe/hof_player_cards.fen" },
    { (SceneList)66, "art/fe/hof_cup_history.fen" },
    { (SceneList)67, "art/fe/hof_award_history.fen" },
    { (SceneList)68, "art/fe/hof_award_history.fen" },
    { (SceneList)69, "art/fe/hof_cup_history.fen" },
    { (SceneList)70, "art/fe/hof_award_history.fen" },
    { (SceneList)71, "art/fe/hof_award_history.fen" },
    { (SceneList)72, "art/fe/hof_cup_history.fen" },
    { (SceneList)73, "art/fe/hof_award_history.fen" },
    { (SceneList)74, "art/fe/hof_award_history.fen" },
    { (SceneList)75, "art/fe/striker_challenge_hub.fen" },
    { (SceneList)76, "art/fe/striker_challenge_hub.fen" },
    { (SceneList)77, "art/fe/striker_times_2.fen" },
    { (SceneList)78, "art/fe/domination_choose_sides.fen" },
    { (SceneList)79, "art/fe/e3howtoholdcontroller.fen" },
    { (SceneList)80, "art/fe/pausemenu_v3.fen" },
    { (SceneList)81, "art/fe/ingame_choose_sides.fen" },
    { (SceneList)82, "art/fe/options_audio_options.fen" },
    { (SceneList)83, "art/fe/options_visual_options.fen" },
    { (SceneList)84, "art/fe/pausemenu101_v3.fen" },
    { (SceneList)85, "art/fe/lesson.fen" },
    { (SceneList)86, "art/fe/strikers_101_lessons_v3.fen" },
    { (SceneList)87, "art/fe/lessonmovieplayer.fen" },
    { (SceneList)88, 0 },
    { OVERLAY_HUD, "art/fe/hud_2.fen" },
    { (SceneList)90, "art/fe/ingame_text.fen" },
    { (SceneList)91, "art/fe/striker_times_2.fen" },
    { (SceneList)92, "art/fe/striker_times_2.fen" },
    { (SceneList)93, "art/fe/online_ranking.fen" },
    { (SceneList)94, "art/fe/striker_times_2.fen" },
    { (SceneList)95, "art/fe/goal_overlay.fen" },
    { (SceneList)96, "art/fe/demo_overlay.fen" },
    { (SceneList)97, "art/fe/igticker.fen" },
    { (SceneList)98, "art/fe/x2_sts.fen" },
    { (SceneList)99, "art/fe/loading_screen.fen" },
    { (SceneList)100, "art/fe/megastrike_metre.fen" },
    { (SceneList)101, "art/fe/pip.fen" },
    { (SceneList)102, "art/fe/SUPER_ABILITY_PRESENTATION.fen" },
    { (SceneList)103, "art/fe/challenge_preview.fen" },
    { (SceneList)104, "art/fe/controller_config.fen" },
    { (SceneList)105, "art/fe/defensive_play.fen" },
    { (SceneList)106, 0 },
};

BaseGameSceneManager::BaseGameSceneManager()
{
    mCurrentStackDepth = 0;
    for (int i = 0; i < MAX_SCENE_DEPTH; ++i)
    {
        m_sceneStack[i] = SCENE_INVALID;
        mBaseSceneHandlerStack[i] = 0;
    }
}

BaseGameSceneManager::~BaseGameSceneManager()
{
    while (mCurrentStackDepth != 0)
    {
        this->Pop();
    }
}

BaseSceneHandler* BaseGameSceneManager::Push(SceneList newscene, ScreenMovement movement, bool popfirst)
{
    if (popfirst)
    {
        Pop();
    }

    const char* filename = SceneEntryTable[newscene].mFenFileName;
    BaseSceneHandler* newHandler = 0;

    if (newscene == SCENE_TITLE && (g_e3_Build || lbl_806E1091))
    {
        filename = "art/fe/sms2_start.fen";
    }

    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        if (newscene == (SceneList)39 || newscene == (SceneList)77 || newscene == (SceneList)91)
        {
            filename = "art/fe/striker_times_jp.fen";
        }
        else if (newscene == (SceneList)104)
        {
            filename = "art/fe/controller_config_jp.fen";
        }
    }

    switch (newscene)
    {
    case SCENE_TITLE:
        newHandler = new (nlMalloc(sizeof(TitleScene), 8, false)) TitleScene(movement);
        break;
    case SCENE_MAIN_MENU:
        newHandler = new (nlMalloc(sizeof(SHMainMenu), 8, false)) SHMainMenu();
        break;
    case SCENE_CHOOSE_CAPTAINS_DOMINATION:
        newHandler = new (nlMalloc(sizeof(ChooseCaptainsSceneV2), 8, false)) ChooseCaptainsSceneV2(ChooseCaptainsSceneV2::ST_DOMINATION, movement);
        break;
    case SCENE_CHOOSE_SIDEKICKS_DOMINATION:
        newHandler = new (nlMalloc(sizeof(ChooseSidekicksSceneV2), 8, false)) ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::ST_DOMINATION, movement);
        break;
    case (SceneList)4:
        newHandler = new (nlMalloc(sizeof(SHChooseSides2), 8, false)) SHChooseSides2(SHChooseSides2::FRIENDLY, movement);
        break;
    case (SceneList)5:
        newHandler = new (nlMalloc(sizeof(StadiumSelectScene), 8, false)) StadiumSelectScene();
        break;
    case SCENE_CHOOSE_CAPTAINS_STRIKER_CUP:
        newHandler = new (nlMalloc(sizeof(ChooseCaptainsSceneV2), 8, false)) ChooseCaptainsSceneV2(ChooseCaptainsSceneV2::ST_STRIKER_CUP, movement);
        break;
    case SCENE_CHOOSE_SIDEKICKS_STRIKER_CUP:
        newHandler = new (nlMalloc(sizeof(ChooseSidekicksSceneV2), 8, false)) ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::ST_STRIKER_CUP, movement);
        break;
    case (SceneList)8:
        newHandler = new (nlMalloc(sizeof(SHChooseSides2), 8, false)) SHChooseSides2(SHChooseSides2::CUP, movement);
        break;
    case (SceneList)9:
        newHandler = new (nlMalloc(sizeof(CupCheaterScene), 8, false)) CupCheaterScene();
        break;
    case SCENE_POPUP_MENU:
        newHandler = new (nlMalloc(sizeof(FEPopupMenu), 8, false)) FEPopupMenu();
        break;
    case (SceneList)13:
        newHandler = new (nlMalloc(sizeof(OptionsScene), 8, false)) OptionsScene();
        break;
    case SCENE_AUDIO_OPTIONS:
        newHandler = new (nlMalloc(sizeof(OptionsAudioMenuV2), 8, false)) OptionsAudioMenuV2(0);
        break;
    case SCENE_VISUAL_OPTIONS:
        newHandler = new (nlMalloc(sizeof(OptionsVisualMenuV2), 8, false)) OptionsVisualMenuV2(0);
        break;
    case (SceneList)16:
        newHandler = new (nlMalloc(sizeof(CrossFaderScene), 8, false)) CrossFaderScene();
        break;
    case SCENE_SUPER_LOADING:
        newHandler = new (nlMalloc(sizeof(SuperLoadingScene), 8, false)) SuperLoadingScene();
        break;
    case SCENE_BOOT_LOADING:
        newHandler = new (nlMalloc(sizeof(BootLoadingScene), 8, false)) BootLoadingScene();
        break;
    case SCENE_BOOT_LOADING_JPN:
        newHandler = new (nlMalloc(sizeof(BootLoadingScene), 8, false)) BootLoadingScene();
        break;
    case (SceneList)20:
        newHandler = new (nlMalloc(sizeof(MoviePlayerScene), 8, false)) MoviePlayerScene();
        break;
    case (SceneList)21:
        newHandler = new (nlMalloc(sizeof(NLGLogoMovieScene), 8, false)) NLGLogoMovieScene();
        break;
    case SCENE_INTRO_MOVIE:
        newHandler = new (nlMalloc(sizeof(IntroMovieScene), 8, false)) IntroMovieScene();
        break;
    case SCENE_CREDITS:
        newHandler = new (nlMalloc(sizeof(CreditScene), 8, false)) CreditScene();
        break;
    case (SceneList)24:
        newHandler = new (nlMalloc(sizeof(NetworkStartScene), 8, false)) NetworkStartScene();
        break;
    case SCENE_ASYNC_LOADING:
        newHandler = new (nlMalloc(sizeof(BaseLoadingScene), 8, false)) BaseLoadingScene();
        break;
    case SCENE_WIDESCREEN_LOADING:
        newHandler = new (nlMalloc(sizeof(MatchLoadingScene), 8, false)) MatchLoadingScene();
        break;
    case SCENE_GAMEPLAY_OPTIONS:
        newHandler = new (nlMalloc(sizeof(SHGameplayOptions), 8, false)) SHGameplayOptions();
        break;
    case SCENE_OPTIONS_CHEATS_LIST:
        newHandler = new (nlMalloc(sizeof(SHOptionsCheatsList), 8, false)) SHOptionsCheatsList();
        break;
    case (SceneList)29:
        newHandler = new (nlMalloc(sizeof(SHNavigation), 8, false)) SHNavigation();
        break;
    case (SceneList)30:
        newHandler = new (nlMalloc(sizeof(SHNavigation), 8, false)) SHNavigation();
        break;
    case (SceneList)31:
        newHandler = new (nlMalloc(sizeof(RoadToStrikersCupHubScene), 8, false)) RoadToStrikersCupHubScene();
        break;
    case (SceneList)32:
        newHandler = new (nlMalloc(sizeof(CupHubScene), 8, false)) CupHubScene();
        break;
    case (SceneList)33:
        newHandler = new (nlMalloc(sizeof(GameResultsScene), 8, false)) GameResultsScene();
        break;
    case (SceneList)34:
        newHandler = new (nlMalloc(sizeof(CupKnockoutScene), 8, false)) CupKnockoutScene();
        break;
    case (SceneList)35:
        newHandler = new (nlMalloc(sizeof(CupFinalRoundsScene), 8, false)) CupFinalRoundsScene();
        break;
    case (SceneList)36:
        newHandler = new (nlMalloc(sizeof(StrikerCupStandingsScene), 8, false)) StrikerCupStandingsScene();
        break;
    case (SceneList)37:
        newHandler = new (nlMalloc(sizeof(StrikerCupAwardsScene), 8, false)) StrikerCupAwardsScene(5);
        break;
    case (SceneList)38:
        newHandler = new (nlMalloc(sizeof(StrikerCupAwardsScene), 8, false)) StrikerCupAwardsScene(6);
        break;
    case (SceneList)39:
        newHandler = new (nlMalloc(sizeof(CupNewsScene), 8, false)) CupNewsScene();
        break;
    case SCENE_ONLINE_MENU:
        newHandler = new (nlMalloc(sizeof(SHOnlineHub), 8, false)) SHOnlineHub();
        break;
    case (SceneList)41:
        newHandler = new (nlMalloc(sizeof(SHOnlinePlayerCount), 8, false)) SHOnlinePlayerCount(SHOnlinePlayerCount::ModeRanked);
        break;
    case (SceneList)42:
        newHandler = new (nlMalloc(sizeof(SHOnlinePlayerCount), 8, false)) SHOnlinePlayerCount(SHOnlinePlayerCount::ModeUnranked);
        break;
    case SCENE_ONLINE_GUEST_CONTROLLER_SELECT:
        newHandler = new (nlMalloc(sizeof(SHOnlineGuestControllerSelect), 8, false)) SHOnlineGuestControllerSelect();
        break;
    case (SceneList)44:
        newHandler = new (nlMalloc(sizeof(SHOnlineInvitePlayers), 8, false)) SHOnlineInvitePlayers();
        break;
    case SCENE_ONLINE_INVITE_PREVIEW:
        newHandler = new (nlMalloc(sizeof(SHOnlineInvitePreview), 8, false)) SHOnlineInvitePreview();
        break;
    case SCENE_ONLINE_RANKING:
        newHandler = new (nlMalloc(sizeof(SHOnlineRanking), 8, false)) SHOnlineRanking();
        break;
    case (SceneList)47:
        newHandler = new (nlMalloc(sizeof(SHOnlineFriends), 8, false)) SHOnlineFriends();
        break;
    case SCENE_ONLINE_FRIEND_CODE_ENTRY:
        newHandler = new (nlMalloc(sizeof(SHOnlineFriendCodeEntry), 8, false)) SHOnlineFriendCodeEntry();
        break;
    case (SceneList)49:
        newHandler = new (nlMalloc(sizeof(SHOnlineMatchmakingDraft), 8, false)) SHOnlineMatchmakingDraft();
        break;
    case (SceneList)50:
        newHandler = new (nlMalloc(sizeof(SHOnlineFriendsDraft), 8, false)) SHOnlineFriendsDraft();
        break;
    case SCENE_ONLINE_LOGIN:
        newHandler = new (nlMalloc(sizeof(SHOnlineLogin), 8, false)) SHOnlineLogin();
        break;
    case SCENE_ONLINE_INVITE_RESPONSE:
        newHandler = new (nlMalloc(sizeof(SHOnlineInviteResponse), 8, false)) SHOnlineInviteResponse();
        break;
    case SCENE_ONLINE_INVITE_STATUS:
        newHandler = new (nlMalloc(sizeof(SHOnlineInviteStatus), 8, false)) SHOnlineInviteStatus();
        break;
    case SCENE_ONLINE_MII_SELECT:
        newHandler = new (nlMalloc(sizeof(SHOnlineMiiSelect), 8, false)) SHOnlineMiiSelect();
        break;
    case SCENE_ONLINE_MII_SELECT_OVERLAY:
        newHandler = new (nlMalloc(sizeof(SHOnlineMiiSelectOverlay), 8, false)) SHOnlineMiiSelectOverlay();
        break;
    case (SceneList)56:
        newHandler = new (nlMalloc(sizeof(SHOnlineFriendsChooseSides), 8, false)) SHOnlineFriendsChooseSides();
        break;
    case (SceneList)57:
        newHandler = new (nlMalloc(sizeof(OnlineConnectionQualityScene), 8, false)) OnlineConnectionQualityScene();
        break;
    case (SceneList)58:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameProfile), 8, false)) SHHallOfFameProfile();
        break;
    case (SceneList)59:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameSummary), 8, false)) SHHallOfFameSummary(14);
        break;
    case (SceneList)60:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameSummary), 8, false)) SHHallOfFameSummary(15);
        break;
    case (SceneList)61:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameSummary), 8, false)) SHHallOfFameSummary(16);
        break;
    case (SceneList)62:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameCup), 8, false)) SHHallOfFameCup(0);
        break;
    case (SceneList)63:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameCup), 8, false)) SHHallOfFameCup(1);
        break;
    case (SceneList)64:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameCup), 8, false)) SHHallOfFameCup(2);
        break;
    case (SceneList)65:
        newHandler = new (nlMalloc(sizeof(SHHallOfFamePlayerCard), 8, false)) SHHallOfFamePlayerCard(13);
        break;
    case (SceneList)66:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(4);
        break;
    case (SceneList)67:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(8);
        break;
    case (SceneList)68:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(7);
        break;
    case (SceneList)69:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(6);
        break;
    case (SceneList)70:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(12);
        break;
    case (SceneList)71:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(11);
        break;
    case (SceneList)72:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(5);
        break;
    case (SceneList)73:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(10);
        break;
    case (SceneList)74:
        newHandler = new (nlMalloc(sizeof(SHHallOfFameHistory), 8, false)) SHHallOfFameHistory(9);
        break;
    case (SceneList)75:
        newHandler = new (nlMalloc(sizeof(ChallengeSelectScene), 8, false)) ChallengeSelectScene(false);
        break;
    case (SceneList)76:
        newHandler = new (nlMalloc(sizeof(ChallengeSelectScene), 8, false)) ChallengeSelectScene(true);
        break;
    case (SceneList)77:
        newHandler = new (nlMalloc(sizeof(SHStrikerTimesChallenge), 8, false)) SHStrikerTimesChallenge();
        break;
    case (SceneList)78:
        newHandler = new (nlMalloc(sizeof(SHChooseSides2), 8, false)) SHChooseSides2(SHChooseSides2::TOURNAMENT, movement);
        break;
    case (SceneList)79:
        newHandler = new (nlMalloc(sizeof(HealthWarningSceneV2), 8, false)) HealthWarningSceneV2();
        break;
    case (SceneList)80:
        newHandler = new (nlMalloc(sizeof(PauseMenuScene), 8, false)) PauseMenuScene();
        break;
    case (SceneList)81:
        newHandler = new (nlMalloc(sizeof(SHChooseSides2), 8, false)) SHChooseSides2(SHChooseSides2::PAUSE, movement);
        break;
    case (SceneList)82:
        newHandler = new (nlMalloc(sizeof(OptionsAudioMenuV2), 8, false)) OptionsAudioMenuV2(1);
        break;
    case (SceneList)83:
        newHandler = new (nlMalloc(sizeof(OptionsVisualMenuV2), 8, false)) OptionsVisualMenuV2(1);
        break;
    case (SceneList)84:
        newHandler = new (nlMalloc(sizeof(PauseMenuScene), 8, false)) PauseMenuScene();
        break;
    case (SceneList)87:
        newHandler = new (nlMalloc(sizeof(LessonMoviePlayerScene), 8, false)) LessonMoviePlayerScene();
        break;
    case OVERLAY_HUD:
        newHandler = new (nlMalloc(sizeof(HUDOverlay), 8, false)) HUDOverlay();
        break;
    case (SceneList)90:
        newHandler = new (nlMalloc(sizeof(InGameTextOverlay), 8, false)) InGameTextOverlay();
        break;
    case (SceneList)91:
        newHandler = new (nlMalloc(sizeof(PausePostGameScene), 8, false)) PausePostGameScene(PausePostGameScene::MODE_RESULTS);
        break;
    case (SceneList)92:
        newHandler = new (nlMalloc(sizeof(PausePostGameScene), 8, false)) PausePostGameScene(PausePostGameScene::MODE_STATISTICS);
        break;
    case (SceneList)93:
        newHandler = new (nlMalloc(sizeof(OnlineRankingOverlay), 8, false)) OnlineRankingOverlay();
        break;
    case (SceneList)94:
        newHandler = new (nlMalloc(sizeof(StrikerTimesOverlay), 8, false)) StrikerTimesOverlay();
        break;
    case (SceneList)95:
        newHandler = new (nlMalloc(sizeof(GoalOverlay), 8, false)) GoalOverlay();
        break;
    case (SceneList)96:
        newHandler = new (nlMalloc(sizeof(DemoOverlay), 8, false)) DemoOverlay();
        break;
    case (SceneList)99:
        newHandler = new (nlMalloc(sizeof(BaseSceneHandler), 8, false)) BaseSceneHandler();
        break;
    case (SceneList)100:
        newHandler = new (nlMalloc(sizeof(MegaStrikeMeterOverlay), 8, false)) MegaStrikeMeterOverlay();
        break;
    case (SceneList)101:
        newHandler = new (nlMalloc(sizeof(PIPOverlay), 8, false)) PIPOverlay();
        break;
    case (SceneList)102:
        newHandler = new (nlMalloc(sizeof(SuperAbilityOverlay), 8, false)) SuperAbilityOverlay();
        break;
    case (SceneList)103:
        newHandler = new (nlMalloc(sizeof(ChallengePreviewOverlay), 8, false)) ChallengePreviewOverlay(movement);
        break;
    case (SceneList)104:
        newHandler = new (nlMalloc(sizeof(ControllerMapOverlay), 8, false)) ControllerMapOverlay();
        break;
    case (SceneList)105:
        newHandler = new (nlMalloc(sizeof(DefensivePlayOverlay), 8, false)) DefensivePlayOverlay();
        break;
    }

    FESceneManager::Instance()->QueueScenePush(newHandler, filename, &VirtualAllocator);

    if (newscene == SCENE_WIDESCREEN_LOADING)
    {
        newscene = SCENE_ASYNC_LOADING;
    }

    m_sceneStack[mCurrentStackDepth] = newscene;
    mBaseSceneHandlerStack[mCurrentStackDepth] = newHandler;
    mCurrentStackDepth++;

    if (g_bEnableGamecubePadMonkey)
    {
        nlPrintf("PAD MONKEY PUSHED: %s\n", filename);
    }

    return newHandler;
}

BaseSceneHandler* BaseGameSceneManager::GetScene(SceneList scene)
{
    BaseSceneHandler* returnValue = 0;

    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (m_sceneStack[i] == scene)
        {
            returnValue = mBaseSceneHandlerStack[i];
            break;
        }
    }

    return returnValue;
}

void BaseGameSceneManager::Pop()
{
    FESceneManager::Instance()->QueueScenePop();
    mBaseSceneHandlerStack[mCurrentStackDepth] = 0;
    mCurrentStackDepth = (mCurrentStackDepth - 1);
}

void BaseGameSceneManager::PopEntireStack()
{
    while (mCurrentStackDepth != 0)
    {
        this->Pop();
    }
}

void BaseGameSceneManager::PopToScene(SceneList scene)
{
    while (mCurrentStackDepth != 0)
    {
        if (GetSceneType(GetCurrentScene()) == scene)
        {
            return;
        }

        Pop();
    }
}

SceneList BaseGameSceneManager::GetSceneType(BaseSceneHandler* scene)
{
    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (mBaseSceneHandlerStack[i] == scene)
        {
            return m_sceneStack[i];
        }
    }
    return SCENE_INVALID;
}

bool BaseGameSceneManager::IsOnStack(SceneList scene)
{
    for (int i = 0; i < mCurrentStackDepth; ++i)
    {
        if (m_sceneStack[i] == scene)
            return true;
    }
    return false;
}

const char* BaseGameSceneManager::GetFileName(SceneList scene)
{
    return SceneEntryTable[scene].mFenFileName;
}

void BaseGameSceneManager::PushLoadingScene(bool popfirst)
{
    if (popfirst)
    {
        this->Pop();
    }

    SuperLoadingScene* scene
        = (SuperLoadingScene*)Push(SCENE_SUPER_LOADING, SCREEN_FORWARD, false);
    scene->mType = SuperLoadingScene::TT_3D_TRANSITION;
}
