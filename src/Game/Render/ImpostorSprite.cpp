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
#include "Game/UnidentifiedStaticStorage.h"


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
    void SetViewMatrix(const nlMatrix4& matrix)
    {
        mView = matrix;
        mDirty = true;
    }

    void SetProjectionMatrix(const nlMatrix4& matrix)
    {
        mProjection = matrix;
        mDirty = true;
    }

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
    float aspect = (float)mWidth / (float)mHeight;
    float fieldOfView = DegreesToRadians(gImpostorFieldOfViewDegrees);
    nlMatrix4 matrix;
    glMatrixPerspective(matrix,
        fieldOfView, aspect,
        gImpostorNearPlane, gImpostorFarPlane);
    mViewInterface->SetProjectionMatrix(matrix);

    float cameraDistance = mCharacter->GetCameraDistance();
    float cameraLookatZ = mCharacter->GetCameraLookatZ();

    nlVector3 target = { 0.0f, 0.0f, cameraLookatZ };

    nlVector3 normalizedDirection;
    nlVec3Normalize(normalizedDirection, *direction);

    nlVector3 eye;
    nlVec3ScaleAdd(eye, -cameraDistance, normalizedDirection, target);

    nlMatrix4 lookAt;
    glMatrixLookAt(lookAt, eye, target, *up);

    float angle = AngUnitsToRad_fromUnsignedShort(mAngle)
        + DegreesToRadians(gImpostorAngleJitterDegrees * mAngleJitter);
    nlMatrix4 rotation;
    nlMakeRotationMatrixZ(rotation, angle);

    float scale = mCharacter->GetScale();
    nlMatrix4 scaleMatrix;
    nlMakeScaleMatrix(scaleMatrix, scale, scale, scale);

    nlMultMatrices(matrix, rotation, lookAt);
    nlMatrix4 result;
    nlMultMatrices(result, scaleMatrix, matrix);
    matrix = result;
    mViewInterface->SetViewMatrix(matrix);
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
    info.unknown10 = GLTargetInfoMode10_1;
    info.format = mUseIntensityAlpha ? GLTargetFormat_7 : GLTargetFormat_0;
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

    int width = mWidth;
    int height = mHeight;
    mView->m_Enabled = true;
    nlSNPrintf(mName, sizeof(mName), gImpostorSpriteNameFormat, name);
    mView->m_Name = mName;
    mView->m_Target = GLViewTarget_Mode9;
    GLView* view = mView;
    view->m_ViewportX = 0;
    view->m_ViewportY = 0;
    view->m_ViewportWidth = width;
    view->m_ViewportHeight = height;

    GLView* activeView;
    unsigned long activeWidth =
        mEnabled ? mWidth : 0;
    unsigned long activeHeight =
        mEnabled ? mHeight : 0;
    activeView = mView;
    activeView->m_ViewportX = 0;
    activeView->m_ViewportY = 0;
    activeView->m_ViewportWidth = activeWidth;
    activeView->m_ViewportHeight = activeHeight;
    bool enabled = mEnabled && !mUnidentified079;
    mView->m_Target =
        enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
}

int ImpostorSprite::Render(GLView* target, Impostor* impostors, bool cached, bool skipCapture)
{
    int rendered;
    GLCompactColourMeshWriter* writer;
    int i;
    Impostor* impostor;

    if (cached)
    {
        if (mMesh != 0
            && mMesh->End())
        {
            target->AttachModel(
                mMesh->GetModel(), gImpostorRenderLayer);
        }
        return 0;
    }

    if (mMesh != 0 && !skipCapture)
    {
        delete mMesh;
        mMesh = 0;
    }

    int count = mNumRenderSlots;
    rendered = 0;
    if (count == 0)
    {
        return 0;
    }

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
        began = writer->Begin(count * 4, GLP_QuadList, allocator);
    }
    else
    {
        began = writer->Begin(count * 6, GLP_TriList, allocator);
    }

    if (began)
    {
        unsigned long texture =
            glGetTargetTexture(mView->GetRenderPair());

        nlMatrix4 viewMatrix;
        target->m_Interface->GetViewMatrix(viewMatrix);
        nlVector3 right;
        nlVector3 up;
        nlVector3 forward;
        nlVec3Set(right,
            viewMatrix.m11, viewMatrix.m21, viewMatrix.m31);
        nlVec3Set(up,
            viewMatrix.m12, viewMatrix.m22, viewMatrix.m32);
        nlVec3Set(forward,
            viewMatrix.m13, viewMatrix.m23, viewMatrix.m33);

        float aspect =
            (float)mWidth / (float)mHeight;
        nlVec3Scale(right, aspect);

        int* slot = mRenderSlots;
        for (i = 0; i < count; ++slot, ++i)
        {
            impostor = &impostors[*slot];
            ImpostorQuad quad;
            BuildQuad(&quad, impostor, &right, &up);

            int vertex;
            if (hasQuads)
            {
                for (vertex = 0; vertex < 4; ++vertex)
                {
                    writer->Texcoord(quad.texcoord[vertex]);
                    writer->Colour(impostor->mColour);
                    writer->Vertex(quad.position[vertex]);
                }
            }
            else
            {
                for (vertex = 0; vertex < 6; ++vertex)
                {
                    int index = sImpostorTriangleIndices[vertex];
                    writer->Texcoord(quad.texcoord[index]);
                    writer->Colour(impostor->mColour);
                    writer->Vertex(quad.position[index]);
                }
            }
            ++rendered;
        }

        if (gDrawImpostorCheckers)
        {
            texture = checkerTexture;
        }
        glTextureBinding* textureState =
            (glTextureBinding*)writer->GetModel()->packets->materialParameters;
        textureState->texture = texture;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (writer->End())
        {
            target->AttachModel(writer->GetModel(), gImpostorRenderLayer);
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
    nlVector3 position = impostor->mPosition;

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
    float angle = 0.0f;
    nlSinCos(&sn, &cs, DegreesToAngle(angle));
    position.z += 0.5f * height;

    nlVector3 a;
    nlVector3 b;
    a.x = cs * right->x + sn * up->x;
    a.y = cs * right->y + sn * up->y;
    a.z = cs * right->z + sn * up->z;
    b.x = -sn * right->x + cs * up->x;
    b.y = -sn * right->y + cs * up->y;
    b.z = -sn * right->z + cs * up->z;
    float halfWidth = 0.5f * width;
    float halfHeight = 0.5f * height;
    nlVec3Scale(a, halfWidth);
    nlVec3Scale(b, halfHeight);

    quad->position[0].x = position.x + a.x + b.x;
    quad->position[0].y = position.y + a.y + b.y;
    quad->position[0].z = position.z + a.z + b.z;
    quad->position[1].x = position.x - a.x + b.x;
    quad->position[1].y = position.y - a.y + b.y;
    quad->position[1].z = position.z - a.z + b.z;
    quad->position[2].x = position.x - a.x - b.x;
    quad->position[2].y = position.y - a.y - b.y;
    quad->position[2].z = position.z - a.z - b.z;
    quad->position[3].x = position.x + a.x - b.x;
    quad->position[3].y = position.y + a.y - b.y;
    quad->position[3].z = position.z + a.z - b.z;
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
