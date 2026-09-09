#include "NL/nlPrint.h"
#include "Game/Render/ImpostorSprite.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/Sys/debug.h"

#include "Game/GL/GLCompactColourMeshWriter.h"
#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorManager.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"


static int sImpostorTriangleIndices[6] = { 0, 1, 2, 0, 2, 3 };
static char sImpostorCheckerTextureName[] = "global/checkers";
static char sImpostorMeshEndError[] = "couldn't end mMesh built by sprites\n";
static char sImpostorMeshBeginError[] = "could not begin a mMesh for sprites\n";

float gImpostorAngleJitterDegrees = 10.0f;
unsigned long gImpostorRenderLayer = 1;
float gImpostorFieldOfViewDegrees = 38.0f;
float gImpostorNearPlane = 0.25f;
float gImpostorFarPlane = 512.0f;
char gImpostorSpriteNameFormat[] = "%s";

bool gDrawImpostorCheckers;
int gNextImpostorSpriteID;
int gImpostorClearColourChannel;


class ImpostorView : public GLViewInterface
{
public:
    void UpdateMatrices() const
    {
        if (mDirty)
        {
            nlInvertMatrix(mInverseView, mView);
            nlMultMatrices(mViewProjection, mView, mProjection);
        }
    }

    virtual void GetViewMatrix(nlMatrix4& matrix) const;
    virtual void GetProjectionMatrix(nlMatrix4& matrix) const;
    virtual void GetInverseViewMatrix(nlMatrix4& matrix) const;
    virtual void GetViewProjectionMatrix(nlMatrix4& matrix) const;
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;

    /* 0x004 */ nlMatrix4 mView;
    /* 0x044 */ nlMatrix4 mProjection;
    /* 0x084 */ mutable nlMatrix4 mInverseView;
    /* 0x0C4 */ mutable nlMatrix4 mViewProjection;
    /* 0x104 */ bool mDirty;
}; // size: 0x108

struct ImpostorQuad
{
    nlVector2 texcoord[4];
    nlVector3 position[4];
}; // size: 0x50

ImpostorSprite::ImpostorSprite(
    ImpostorCharacter* character, int texture, int capacity, int width,
    int height)
    : mCaptureSuspended(false)
    , mID(-1)
    , mTextureIndex(texture)
    , mCharacter(character)
    , mWidth(width)
    , mHeight(height)
    , mImpostorSlots(0)
    , mNumImpostorSlots(0)
    , mCapacity(capacity)
    , mViewInterface(0)
    , mView(0)
    , mRenderPair()
    , mMesh(0)
    , mEnabled(false)
    , mUnidentified079(false)
    , mAngle(0)
    , mNumRenderSlots(0)
    , mUseIntensityAlpha(false)
{
    mEnabled = ImpostorManager::GetInstance()->mEnabled;
    mViewInterface =
        new (8, false) ImpostorView;
    mAngleJitter = nlRandomf(-1.0f, 1.0f, &nlDefaultSeed);

    unsigned long allocationSize = capacity * sizeof(int);
    mImpostorSlots = (int*)nlMalloc(allocationSize, 8, false);
    mRenderSlots = (int*)nlMalloc(allocationSize, 8, false);

    mID = gNextImpostorSpriteID++;
}

ImpostorSprite::~ImpostorSprite()
{
    if (mViewInterface != 0)
    {
        delete mViewInterface;
    }

    ImpostorManager::GetInstance()->mParentView
        ->RemoveChild(mView);
    if (mView != 0)
    {
        delete mView;
    }

    ::operator delete(mImpostorSlots);
    mNumImpostorSlots = 0;
    ::operator delete(mRenderSlots);
    mNumRenderSlots = 0;

    glDestroyTarget(&mRenderPair);
    if (mMesh != 0)
    {
        delete mMesh;
    }

    mViewInterface = 0;
    mView = 0;
}

void ImpostorSprite::Initialize(const char* name)
{
    CreateRenderTarget(name);
}

void ImpostorSprite::UpdateView(const nlVector3* direction, const nlVector3* up)
{
    nlMatrix4 projection;
    glMatrixPerspective(projection,
        (3.1415927f * gImpostorFieldOfViewDegrees) / 180.0f,
        (float)mWidth / (float)mHeight,
        gImpostorNearPlane, gImpostorFarPlane);
    mViewInterface->mProjection = projection;
    mViewInterface->mDirty = true;

    float cameraDistance = mCharacter->GetCameraDistance();
    float cameraLookatZ = mCharacter->GetCameraLookatZ();

    nlVector3 target;
    target.x = 0.0f;
    target.y = 0.0f;
    target.z = cameraLookatZ;

    float inverseLength = nlRecipSqrt(direction->x * direction->x
            + direction->y * direction->y + direction->z * direction->z,
        true);
    nlVector3 normalizedDirection;
    normalizedDirection.x = inverseLength * direction->x;
    normalizedDirection.y = inverseLength * direction->y;
    normalizedDirection.z = inverseLength * direction->z;

    nlVector3 eye;
    eye.x = -cameraDistance * normalizedDirection.x + target.x;
    eye.y = -cameraDistance * normalizedDirection.y + target.y;
    eye.z = -cameraDistance * normalizedDirection.z + target.z;

    nlMatrix4 lookAt;
    glMatrixLookAt(lookAt, eye, target, *up);

    float angle = (float)mAngle * 0.0000958738f
        + (3.1415927f * (gImpostorAngleJitterDegrees * mAngleJitter)) / 180.0f;
    nlMatrix4 rotation;
    nlMakeRotationMatrixZ(rotation, angle);

    float scale = mCharacter->GetScale();
    nlMatrix4 scaleMatrix;
    nlMakeScaleMatrix(scaleMatrix, scale, scale, scale);

    nlMatrix4 rotatedView;
    nlMultMatrices(rotatedView, rotation, lookAt);
    nlMatrix4 view;
    nlMultMatrices(view, scaleMatrix, rotatedView);
    mViewInterface->mView = view;
    mViewInterface->mDirty = true;
    mViewInterface->UpdateMatrices();
}

void ImpostorSprite::UpdateViewport()
{
    GLView* view;
    unsigned long width =
        mEnabled ? mWidth : 0;
    unsigned long height =
        mEnabled ? mHeight : 0;

    view = mView;
    view->m_ViewportX = 0;
    view->m_ViewportY = 0;
    view->m_ViewportWidth = width;
    view->m_ViewportHeight = height;

    bool enabled = mEnabled && !mUnidentified079;
    mView->m_Target =
        enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
}

void ImpostorSprite::SuspendCapture()
{
    if (!mCaptureSuspended)
    {
        mCaptureSuspended = true;
        mView->m_Target = GLViewTarget_None;
    }
}

void ImpostorSprite::ResumeCapture()
{
    if (mCaptureSuspended)
    {
        mCaptureSuspended = false;
        bool enabled = mEnabled && !mUnidentified079;
        mView->m_Target =
            enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
    }
}

void ImpostorSprite::CreateRenderTarget(const char* name)
{
    GLTargetInfo info;
    nlZeroMemory(&info, sizeof(info));
    info.width = mWidth;
    info.height = mHeight;
    info.unknown10 = 1;
    info.format = mUseIntensityAlpha ? 7 : 0;
    info.unknown18 = 7;
    info.unknown1C = 4;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    info.colour[gImpostorClearColourChannel % 3] = 0x40;

    mRenderPair = glCreateTarget(name, &info);
    mView = new (8, false) GLView(
        mViewInterface, mRenderPair, GLViewSort_None);
    mView->m_ClearColour = false;
    mView->m_ClearDepth = false;
    mView->m_Unknown32 = false;
    mView->m_Enabled = true;

    int width = mWidth;
    int height = mHeight;
    nlSNPrintf(mName, sizeof(mName), gImpostorSpriteNameFormat, name);
    mView->m_Name = mName;
    mView->m_Target = GLViewTarget_Mode9;
    mView->m_ViewportX = 0;
    mView->m_ViewportY = 0;
    mView->m_ViewportWidth = width;
    mView->m_ViewportHeight = height;

    unsigned long activeWidth =
        mEnabled ? mWidth : 0;
    unsigned long activeHeight =
        mEnabled ? mHeight : 0;
    mView->m_ViewportX = 0;
    mView->m_ViewportY = 0;
    mView->m_ViewportWidth = activeWidth;
    mView->m_ViewportHeight = activeHeight;
    bool enabled = mEnabled && !mUnidentified079;
    mView->m_Target =
        enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
}

static inline void WriteImpostorVertex(GLCompactColourMeshWriter* writer,
    const ImpostorQuad& quad, int index,
    const Impostor& impostor)
{
    *writer->texcoord++ = (short)(quad.texcoord[index].x * 1024.0f);
    *writer->texcoord++ = (short)(quad.texcoord[index].y * 1024.0f);
    *writer->colour++ = *(const u32*)&impostor.mColour;
    *writer->position++ = (short)(quad.position[index].x * 64.0f);
    *writer->position++ = (short)(quad.position[index].y * 64.0f);
    *writer->position++ = (short)(quad.position[index].z * 64.0f);
}


int ImpostorSprite::Render(GLView* target, Impostor* impostors, bool cached, bool skipCapture)
{
    if (cached)
    {
        if (mMesh != 0
            && mMesh->End())
        {
            target->AttachModel(
                mMesh->model, gImpostorRenderLayer);
        }
        return 0;
    }

    if (mMesh != 0 && !skipCapture)
    {
        delete mMesh;
        mMesh = 0;
    }

    int count = mNumRenderSlots;
    int rendered = 0;
    if (count == 0)
    {
        return 0;
    }

    GLCompactColourMeshWriter* writer;
    if (skipCapture)
    {
        writer = new (8, false) GLCompactColourMeshWriter;
    }
    else
    {
        mMesh = new (8, false) GLCompactColourMeshWriter;
        writer = mMesh;
    }

    bool hasQuads = glHasQuads();
    static unsigned long checkerTexture = glGetTexture(sImpostorCheckerTextureName);

    void* allocator;
    if (skipCapture)
    {
        allocator = 0;
    }
    else
    {
        ImpostorManager* manager = ImpostorManager::GetInstance();
        allocator = manager->mResources[manager->mCurrentResource];
    }

    bool began;
    if (hasQuads)
    {
        began = writer->Begin( count * 4, 3, allocator);
    }
    else
    {
        began = writer->Begin( count * 6, 0, allocator);
    }

    if (began)
    {
        unsigned long texture =
            glGetTargetTexture(mView->GetRenderPair());

        nlMatrix4 viewMatrix;
        target->m_Interface->GetViewMatrix(viewMatrix);
        nlVector3 right;
        right.x = viewMatrix.m11;
        right.y = viewMatrix.m21;
        right.z = viewMatrix.m31;
        nlVector3 up;
        up.x = viewMatrix.m12;
        up.y = viewMatrix.m22;
        up.z = viewMatrix.m32;

        float aspect =
            (float)mWidth / (float)mHeight;
        right.x *= aspect;
        right.y *= aspect;
        right.z *= aspect;

        int* slot = mRenderSlots;
        for (int i = 0; i < count; ++i, ++slot)
        {
            Impostor* impostor = &impostors[*slot];
            ImpostorQuad quad;
            BuildQuad(&quad, impostor, &right, &up);

            if (hasQuads)
            {
                for (int vertex = 0; vertex < 4; ++vertex)
                {
                    WriteImpostorVertex(writer, quad, vertex, *impostor);
                }
            }
            else
            {
                for (int vertex = 0; vertex < 6; ++vertex)
                {
                    WriteImpostorVertex(
                        writer, quad, sImpostorTriangleIndices[vertex], *impostor);
                }
            }
            ++rendered;
        }

        if (gDrawImpostorCheckers)
        {
            texture = checkerTexture;
        }
        glTextureBinding* textureState =
            (glTextureBinding*)writer->model->packets->unknown20;
        textureState->texture = texture;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (writer->End())
        {
            target->AttachModel(writer->model, gImpostorRenderLayer);
        }
        else
        {
            tDebugPrintManager::Print(DC_RENDER, sImpostorMeshEndError);
        }
    }
    else
    {
        tDebugPrintManager::Print(DC_RENDER, sImpostorMeshBeginError);
    }

    if (skipCapture)
    {
        delete writer;
    }
    return rendered;
}

void ImpostorSprite::ClearRenderSlots()
{
    mNumRenderSlots = 0;
}

void ImpostorSprite::QueueAllSlots()
{
    int i = 0;
    for (; i < mNumImpostorSlots; ++i)
    {
        mRenderSlots[i] = mImpostorSlots[i];
    }
    mNumRenderSlots = mNumImpostorSlots;
}

void ImpostorSprite::QueueSlot(int slot)
{
    if (mNumRenderSlots == mCapacity)
    {
        return;
    }
    mRenderSlots[mNumRenderSlots] = slot;
    ++mNumRenderSlots;
}

unsigned long ImpostorSprite::GetTexture()
{
    return glGetTargetTexture(mView->GetRenderPair());
}

bool ImpostorSprite::AddImpostorSlot(int slot)
{
    if (mNumImpostorSlots == mCapacity)
    {
        return false;
    }
    mImpostorSlots[mNumImpostorSlots] = slot;
    ++mNumImpostorSlots;
    return true;
}

void ImpostorSprite::ClearImpostorSlots()
{
    mNumImpostorSlots = 0;
}

void ImpostorSprite::BuildQuad(ImpostorQuad* quad, const Impostor* impostor, const nlVector3* right, const nlVector3* up)
{
    float sizeScale =
        ImpostorManager::GetInstance()->GetImpostorSizeScale();
    float width = sizeScale * impostor->mWidth;
    float height = sizeScale * impostor->mHeight;

    quad->texcoord[0].x = 1.0f;
    quad->texcoord[0].y = 0.0f;
    quad->texcoord[1].x = 0.0f;
    quad->texcoord[1].y = 0.0f;
    quad->texcoord[2].x = 0.0f;
    quad->texcoord[2].y = 1.0f;
    quad->texcoord[3].x = 1.0f;
    quad->texcoord[3].y = 1.0f;

    float sn;
    float cs;
    nlSinCos(&sn, &cs, 0);

    nlVector3 a;
    a.x = 0.5f * width * (cs * right->x + sn * up->x);
    a.y = 0.5f * width * (cs * right->y + sn * up->y);
    a.z = 0.5f * width * (cs * right->z + sn * up->z);

    nlVector3 b;
    b.x = 0.5f * height * (-sn * right->x + cs * up->x);
    b.y = 0.5f * height * (-sn * right->y + cs * up->y);
    b.z = 0.5f * height * (-sn * right->z + cs * up->z);

    quad->position[0].x = impostor->mPosition.x + a.x + b.x;
    quad->position[0].y = impostor->mPosition.y + a.y + b.y;
    quad->position[0].z = impostor->mPosition.z + a.z + b.z;
    quad->position[1].x = impostor->mPosition.x - a.x + b.x;
    quad->position[1].y = impostor->mPosition.y - a.y + b.y;
    quad->position[1].z = impostor->mPosition.z - a.z + b.z;
    quad->position[2].x = impostor->mPosition.x - a.x - b.x;
    quad->position[2].y = impostor->mPosition.y - a.y - b.y;
    quad->position[2].z = impostor->mPosition.z - a.z - b.z;
    quad->position[3].x = impostor->mPosition.x + a.x - b.x;
    quad->position[3].y = impostor->mPosition.y + a.y - b.y;
    quad->position[3].z = impostor->mPosition.z + a.z - b.z;
}

int ImpostorSprite::CalculateRenderChecksum()
{
    int sum = 0;
    for (int i = 0; i < mNumRenderSlots; ++i)
    {
        sum += mRenderSlots[i];
    }
    return sum;
}

void ImpostorView::GetViewProjectionMatrix(
    nlMatrix4& matrix) const
{
    UpdateMatrices();
    matrix = mViewProjection;
}

void ImpostorView::GetInverseViewMatrix(
    nlMatrix4& matrix) const
{
    UpdateMatrices();
    matrix = mInverseView;
}

void ImpostorView::GetProjectionMatrix(
    nlMatrix4& matrix) const
{
    matrix = mProjection;
}

const nlMatrix4*
ImpostorView::GetProjectionMatrix() const
{
    return &mProjection;
}

void ImpostorView::GetViewMatrix(nlMatrix4& matrix) const
{
    matrix = mView;
}

const nlMatrix4* ImpostorView::GetViewMatrix() const
{
    return &mView;
}
