#include "Game/Camera/MatrixEffectCam.h"

#include "NL/gl/glMatrix.h"

extern float lbl_806DC510;
extern bool lbl_806E0F18;

void MatrixEffectCam::Update(float dt)
{
    nlVector3 up;
    up.x = 0.0f;
    up.y = 0.0f;
    up.z = 1.0f;

    if (!lbl_806E0F18)
    {
        nlVector3 currentCameraPosition = mCameraPosition;
        nlVec3ScaleAdd(mCameraPosition, lbl_806DC510, mUnidentified078, mTargetPosition);
        nlVec3WeightedSum(mCameraPosition, 0.85f, currentCameraPosition, 0.15f, mCameraPosition);
        glMatrixLookAt(mViewMatrix, mCameraPosition, mTargetPosition, up);
    }
}

MatrixEffectCam::~MatrixEffectCam()
{
}

MatrixEffectCam::MatrixEffectCam()
{
    nlVec3Set(mCameraPosition, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mTargetPosition, 20.0f, 0.0f, 0.0f);
    nlVec3Set(mUnidentified078, -0.98f, 0.0f, 0.2f);
    Update(0.0f);
}
