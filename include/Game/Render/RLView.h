#ifndef GAME_RENDER_RL_VIEW_H
#define GAME_RENDER_RL_VIEW_H

#include "NL/gl/glView.h"
#include "types.h"

class glModel;

enum eCLV
{
    eCLV_ImpostorTexture = 0,
    eCLV_ShadowTexture = 1,
    eCLV_GrabTexture = 2,
    eCLV_PictureInPicture = 3,
    eCLV_PictureInPictureAlpha = 4,
    eCLV_NoFog = 5,
    eCLV_Shadowed = 6,
    eCLV_WorldShadowed = 7,
    eCLV_Unshadowed = 8,
    eCLV_MegastrikeBackground = 9,
    eCLV_ImpostorOut = 10,
    eCLV_Characters = 11,
    eCLV_PeachPhoto3D = 12,
    eCLV_MoreCharacters = 13,
    eCLV_WorldAlphaBlended = 14,
    eCLV_HighRange3D = 15,
    eCLV_HighRange3DNoFog = 16,
    eCLV_HighRangeChain = 17,
    eCLV_HighRange2D = 18,
    eCLV_BigBlackPolygon = 19,
    eCLV_ShadowVolume = 20,
    eCLV_ShadowVolumeBlend = 21,
    eCLV_UnsortedPerspective = 22,
    eCLV_DepthOfField = 23,
    eCLV_LingeringParticles = 24,
    eCLV_Particles = 25,
    eCLV_BallChargeAlphaBlended = 26,
    eCLV_CoPlanar = 27,
    eCLV_InvisiblePlane = 28,
    eCLV_ElectricFence = 29,
    eCLV_PreWarble = 30,
    eCLV_Warble = 31,
    eCLV_WarbleBlend = 32,
    eCLV_CameraSpace = 33,
    eCLV_ScreenBlur = 34,
    eCLV_ScreenBlur2 = 35,
    eCLV_ScreenGrab = 36,
    eCLV_FrontEnd = 37,
    eCLV_UnsortedOrtho640 = 38,
    eCLV_UnsortedSquareOrtho = 39,
    eCLV_Transitions3D = 40,
    eCLV_Transitions = 41,
    eCLV_Anark3D_BG = 42,
    eCLV_Anark = 43,
    eCLV_Anark3D_FG = 44,
    eCLV_HomeButtonFadeOut = 45,
    eCLV_Debug = 46,
    eCLV_DebugSquare = 47,
    eCLV_Num = 48,
};

class RLView : public GLView
{
public:
    RLView(GLViewInterface* viewInterface, const GLRenderPair& renderPair, GLViewSortMode sortMode);
    virtual ~RLView();

    virtual void BeginRender();
    virtual void EndRender();
    virtual void BeginPacket(const glModelPacket* packet);
    virtual void EndPacket(const glModelPacket* packet);

    /* 0x58 */ bool mZWriteEnabled;
    /* 0x59 */ bool mCoPlanarEnabled;
}; // size 0x5C

#endif // GAME_RENDER_RL_VIEW_H
