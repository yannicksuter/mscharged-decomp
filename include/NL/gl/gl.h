#ifndef NL_GL_GL_H
#define NL_GL_GL_H

enum eGLView
{
    GLV_ShadowTexture = 0,
    GLV_GrabTexture = 1,
    GLV_Skybox = 2,
    GLV_Shadowed = 3,
    GLV_Shadow0 = 4,
    GLV_ShadowBlend0 = 5,
    GLV_WorldShadowed = 6,
    GLV_Unshadowed = 7,
    GLV_BigBlackPolygon = 8,
    GLV_Warble = 9,
    GLV_WarbleBlend = 10,
    GLV_Characters = 11,
    GLV_CoPlanar0 = 12,
    GLV_CoPlanar = 13,
    GLV_Shadow1 = 14,
    GLV_ShadowBlend1 = 15,
    GLV_UnsortedPerspective = 16,
    GLV_DepthOfField = 17,
    GLV_LingeringParticles = 18,
    GLV_Particles = 19,
    GLV_InvisiblePlane = 20,
    GLV_ElectricFence = 21,
    GLV_CameraSpace = 22,
    GLV_ScreenBlur = 23,
    GLV_ScreenBlur2 = 24,
    GLV_ScreenGrab = 25,
    GLV_FrontEnd = 26,
    GLV_UnsortedOrtho = 27,
    GLV_Transitions3D = 28,
    GLV_Transitions = 29,
    GLV_Anark3D_BG = 30,
    GLV_Anark = 31,
    GLV_Anark3D_FG = 32,
    GLV_Debug = 33,
    GLV_Num = 34,
};

void glEndFrame();
void glDiscardFrame(int count);
void glSendFrame();

#endif // NL_GL_GL_H
