#ifndef UNCLASSIFIED_TU_8024D92C_H
#define UNCLASSIFIED_TU_8024D92C_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feAsyncImage.h"
#include "unclassified/tu_8022EF84.h"

class TU8024D92CScene : public BaseSceneHandler
{
public:
    TU8024D92CScene(int mode);
    virtual ~TU8024D92CScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x01C */ int mMode;
    /* 0x020 */ int mUnidentified020;
    /* 0x024 */ TU8022EF84Component mNavigation;
    /* 0x0FC */ int mUnidentified0FC[4];
    /* 0x10C */ bool mUnidentified10C;
    /* 0x10D */ bool mUnidentified10D;
    /* 0x10E */ unsigned short mUnidentified10E[0x20];
    /* 0x14E */ unsigned short mUnidentified14E[0x100];
    /* 0x34E */ unsigned short mUnidentified34E[0x20];
    /* 0x38E */ unsigned char mPadding38E[2];
    /* 0x390 */ AsyncImage mUnidentified390;
    /* 0x430 */ AsyncImage mUnidentified430;
    /* 0x4D0 */ bool mUnidentified4D0;
    /* 0x4D1 */ bool mUnidentified4D1;
    /* 0x4D2 */ unsigned char mPadding4D2[2];
}; // size 0x4D4

#endif // UNCLASSIFIED_TU_8024D92C_H
