#include "Game/Render/ImpostorCharacter.h"

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

extern "C" int nlSNPrintf(char*, unsigned long, const char*, ...);
extern "C" void fn_8004F594(int category, const char* format, ...);

struct State_802A7C90
{
    int count;
    glModel* model;
    void* resource;
    short* position;
    short* texcoord;
    u32* colour;
};

extern "C" void fn_802A7C90(State_802A7C90* writer);
extern "C" void* fn_802A7CB0(State_802A7C90* writer, int shouldDelete);
extern "C" bool fn_802A7CF0(
    State_802A7C90* writer, int vertexCount, int primitive, void* allocator);
extern "C" bool fn_802A7E9C(State_802A7C90* writer);

static int lbl_8052E778[6] = { 0, 1, 2, 0, 2, 3 };
static char lbl_8052E790[] = "global/checkers";
static char lbl_8052E7A0[] = "couldn't end mMesh built by sprites\n";
static char lbl_8052E7C8[] = "could not begin a mMesh for sprites\n";

extern "C"
{
float lbl_806DF410 = 10.0f;
unsigned long lbl_806DF414 = 1;
float lbl_806DF418 = 38.0f;
float lbl_806DF41C = 0.25f;
float lbl_806DF420 = 512.0f;
char lbl_806DF424[] = "%s";

bool lbl_806E1F48;
int lbl_806E1F4C;
int lbl_806E1F50;
}

class UnidentifiedImpostorView_802D4290 : public GLViewInterface
{
public:
    void UpdateMatrices()
    {
        if (mDirty)
        {
            nlInvertMatrix(mInverseView, mView);
            nlMultMatrices(mViewProjection, mView, mProjection);
        }
    }

    virtual void GetViewMatrix(nlMatrix4& matrix);
    virtual void GetProjectionMatrix(nlMatrix4& matrix);
    virtual void GetInverseViewMatrix(nlMatrix4& matrix);
    virtual void GetViewProjectionMatrix(nlMatrix4& matrix);
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;

    /* 0x004 */ nlMatrix4 mView;
    /* 0x044 */ nlMatrix4 mProjection;
    /* 0x084 */ nlMatrix4 mInverseView;
    /* 0x0C4 */ nlMatrix4 mViewProjection;
    /* 0x104 */ bool mDirty;
}; // size: 0x108

struct UnidentifiedTargetInfo_802D48E4
{
    UnidentifiedTargetInfo_802D48E4()
        : unknown08(0)
        , unknown0C(0)
    {
    }

    /* 0x00 */ u32 height;
    /* 0x04 */ u32 width;
    /* 0x08 */ u32 unknown08;
    /* 0x0C */ u32 unknown0C;
    /* 0x10 */ u32 unknown10;
    /* 0x14 */ u32 format;
    /* 0x18 */ u32 unknown18;
    /* 0x1C */ u32 unknown1C;
    /* 0x20 */ u32 unknown20;
    /* 0x24 */ u8 colour[4];
}; // size: 0x28

struct UnidentifiedImpostorQuad_802D511C
{
    nlVector2 texcoord[4];
    nlVector3 position[4];
}; // size: 0x50

ImpostorSprite_802D4290::ImpostorSprite_802D4290(
    ImpostorCharacter* character, int texture, int capacity, int width,
    int height)
    : mUnidentified000(false)
    , mUnidentified044(-1)
    , mUnidentified048(texture)
    , mUnidentified04C(character)
    , mUnidentified050(width)
    , mUnidentified054(height)
    , mUnidentified058(0)
    , mUnidentified05C(0)
    , mUnidentified060(capacity)
    , mUnidentified064(0)
    , mUnidentified068(0)
    , mUnidentified06C()
    , mUnidentified074(0)
    , mUnidentified078(false)
    , mUnidentified079(false)
    , mAngle(0)
    , mUnidentified084(0)
    , mUnidentified088(false)
{
    mUnidentified078 = ImpostorManager::GetInstance()->mEnabled;
    mUnidentified064 =
        new (8, false) UnidentifiedImpostorView_802D4290;
    mUnidentified07C = nlRandomf(-1.0f, 1.0f, &nlDefaultSeed);

    unsigned long allocationSize = capacity * sizeof(int);
    mUnidentified058 = (int*)nlMalloc(allocationSize, 8, false);
    mUnidentified080 = (int*)nlMalloc(allocationSize, 8, false);

    mUnidentified044 = lbl_806E1F4C++;
}

ImpostorSprite_802D4290::~ImpostorSprite_802D4290()
{
    if (mUnidentified064 != 0)
    {
        delete mUnidentified064;
    }

    ((GLView*)ImpostorManager::GetInstance()->mpRegistry)
        ->RemoveChild(mUnidentified068);
    if (mUnidentified068 != 0)
    {
        delete mUnidentified068;
    }

    ::operator delete(mUnidentified058);
    mUnidentified05C = 0;
    ::operator delete(mUnidentified080);
    mUnidentified084 = 0;

    fn_802CDA14(&mUnidentified06C);
    if (mUnidentified074 != 0)
    {
        fn_802A7CB0(mUnidentified074, 1);
    }

    mUnidentified064 = 0;
    mUnidentified068 = 0;
}

extern "C" void fn_802D4480(
    ImpostorSprite_802D4290* sprite, const char* name)
{
    fn_802D48E4(sprite, name);
}

extern "C" void fn_802D4484(ImpostorSprite_802D4290* sprite,
    const nlVector3* direction, const nlVector3* up)
{
    nlMatrix4 projection;
    glMatrixPerspective(projection,
        (3.1415927f * lbl_806DF418) / 180.0f,
        (float)sprite->mUnidentified050 / (float)sprite->mUnidentified054,
        lbl_806DF41C, lbl_806DF420);
    sprite->mUnidentified064->mProjection = projection;
    sprite->mUnidentified064->mDirty = true;

    float cameraDistance = sprite->mUnidentified04C->GetCameraDistance();
    float cameraLookatZ = sprite->mUnidentified04C->GetCameraLookatZ();

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

    float angle = (float)sprite->mAngle * 0.0000958738f
        + (3.1415927f * (lbl_806DF410 * sprite->mUnidentified07C)) / 180.0f;
    nlMatrix4 rotation;
    nlMakeRotationMatrixZ(rotation, angle);

    float scale = sprite->mUnidentified04C->GetScale();
    nlMatrix4 scaleMatrix;
    nlMakeScaleMatrix(scaleMatrix, scale, scale, scale);

    nlMatrix4 rotatedView;
    nlMultMatrices(rotatedView, rotation, lookAt);
    nlMatrix4 view;
    nlMultMatrices(view, scaleMatrix, rotatedView);
    sprite->mUnidentified064->mView = view;
    sprite->mUnidentified064->mDirty = true;
    sprite->mUnidentified064->UpdateMatrices();
}

extern "C" void fn_802D47F8(ImpostorSprite_802D4290* sprite)
{
    unsigned long width =
        sprite->mUnidentified078 ? sprite->mUnidentified050 : 0;
    unsigned long height =
        sprite->mUnidentified078 ? sprite->mUnidentified054 : 0;

    GLView* view = sprite->mUnidentified068;
    view->m_ViewportX = 0;
    view->m_ViewportY = 0;
    view->m_ViewportWidth = width;
    view->m_ViewportHeight = height;

    bool enabled = sprite->mUnidentified078 && !sprite->mUnidentified079;
    sprite->mUnidentified068->m_Target =
        enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
}

extern "C" void fn_802D4874(ImpostorSprite_802D4290* sprite)
{
    if (!sprite->mUnidentified000)
    {
        sprite->mUnidentified000 = true;
        sprite->mUnidentified068->m_Target = GLViewTarget_None;
    }
}

extern "C" void fn_802D4898(ImpostorSprite_802D4290* sprite)
{
    if (sprite->mUnidentified000)
    {
        sprite->mUnidentified000 = false;
        bool enabled = sprite->mUnidentified078 && !sprite->mUnidentified079;
        sprite->mUnidentified068->m_Target =
            enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
    }
}

extern "C" void fn_802D48E4(
    ImpostorSprite_802D4290* sprite, const char* name)
{
    UnidentifiedTargetInfo_802D48E4 info;
    nlZeroMemory(&info, sizeof(info));
    info.width = sprite->mUnidentified050;
    info.height = sprite->mUnidentified054;
    info.unknown10 = 1;
    info.format = sprite->mUnidentified088 ? 7 : 0;
    info.unknown18 = 7;
    info.unknown1C = 4;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    info.colour[lbl_806E1F50 % 3] = 0x40;

    sprite->mUnidentified06C = fn_802CD884(name, &info);
    sprite->mUnidentified068 = new (8, false) GLView(
        sprite->mUnidentified064, sprite->mUnidentified06C, GLViewSort_None);
    sprite->mUnidentified068->m_Unknown33 = false;
    sprite->mUnidentified068->m_ClearColour = false;
    sprite->mUnidentified068->m_ClearDepth = false;
    sprite->mUnidentified068->m_Enabled = true;

    int width = sprite->mUnidentified050;
    int height = sprite->mUnidentified054;
    nlSNPrintf(sprite->mName, sizeof(sprite->mName), lbl_806DF424, name);
    sprite->mUnidentified068->m_Name = sprite->mName;
    sprite->mUnidentified068->m_Target = GLViewTarget_Mode9;
    sprite->mUnidentified068->m_ViewportX = 0;
    sprite->mUnidentified068->m_ViewportY = 0;
    sprite->mUnidentified068->m_ViewportWidth = width;
    sprite->mUnidentified068->m_ViewportHeight = height;

    unsigned long activeWidth =
        sprite->mUnidentified078 ? sprite->mUnidentified050 : 0;
    unsigned long activeHeight =
        sprite->mUnidentified078 ? sprite->mUnidentified054 : 0;
    sprite->mUnidentified068->m_ViewportX = 0;
    sprite->mUnidentified068->m_ViewportY = 0;
    sprite->mUnidentified068->m_ViewportWidth = activeWidth;
    sprite->mUnidentified068->m_ViewportHeight = activeHeight;
    bool enabled = sprite->mUnidentified078 && !sprite->mUnidentified079;
    sprite->mUnidentified068->m_Target =
        enabled ? GLViewTarget_Mode9 : GLViewTarget_None;
}

static inline State_802A7C90* AllocateWriter_802D4290()
{
    State_802A7C90* writer =
        (State_802A7C90*)nlMalloc(sizeof(State_802A7C90), 8, false);
    if (writer != 0)
    {
        fn_802A7C90(writer);
    }
    return writer;
}

static inline void WriteVertex_802D4290(State_802A7C90* writer,
    const UnidentifiedImpostorQuad_802D511C& quad, int index,
    const Impostor& impostor)
{
    *writer->texcoord++ = (short)(quad.texcoord[index].x * 1024.0f);
    *writer->texcoord++ = (short)(quad.texcoord[index].y * 1024.0f);
    *writer->colour++ = *(const u32*)&impostor.mColour;
    *writer->position++ = (short)(quad.position[index].x * 64.0f);
    *writer->position++ = (short)(quad.position[index].y * 64.0f);
    *writer->position++ = (short)(quad.position[index].z * 64.0f);
}

extern "C" void fn_802D511C(ImpostorSprite_802D4290* sprite,
    UnidentifiedImpostorQuad_802D511C* quad, const Impostor* impostor,
    const nlVector3* right, const nlVector3* up);

extern "C" int fn_802D4AEC(ImpostorSprite_802D4290* sprite,
    GLView* target, Impostor* impostors, bool cached, bool skipCapture)
{
    if (cached)
    {
        if (sprite->mUnidentified074 != 0
            && fn_802A7E9C(sprite->mUnidentified074))
        {
            target->AttachModel(
                sprite->mUnidentified074->model, lbl_806DF414);
        }
        return 0;
    }

    if (sprite->mUnidentified074 != 0 && !skipCapture)
    {
        fn_802A7CB0(sprite->mUnidentified074, 1);
        sprite->mUnidentified074 = 0;
    }

    int count = sprite->mUnidentified084;
    int rendered = 0;
    if (count == 0)
    {
        return 0;
    }

    State_802A7C90* writer;
    if (skipCapture)
    {
        writer = AllocateWriter_802D4290();
    }
    else
    {
        sprite->mUnidentified074 = AllocateWriter_802D4290();
        writer = sprite->mUnidentified074;
    }

    bool hasQuads = glHasQuads();
    static unsigned long checkerTexture = glGetTexture(lbl_8052E790);

    void* allocator;
    if (skipCapture)
    {
        allocator = 0;
    }
    else
    {
        ImpostorManager* manager = ImpostorManager::GetInstance();
        allocator = manager->mViews[manager->mCurrentView];
    }

    bool began;
    if (hasQuads)
    {
        began = fn_802A7CF0(writer, count * 4, 3, allocator);
    }
    else
    {
        began = fn_802A7CF0(writer, count * 6, 0, allocator);
    }

    if (began)
    {
        unsigned long texture =
            fn_802CDAA8(sprite->mUnidentified068->GetRenderPair());

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
            (float)sprite->mUnidentified050 / (float)sprite->mUnidentified054;
        right.x *= aspect;
        right.y *= aspect;
        right.z *= aspect;

        int* slot = sprite->mUnidentified080;
        for (int i = 0; i < count; ++i, ++slot)
        {
            Impostor* impostor = &impostors[*slot];
            UnidentifiedImpostorQuad_802D511C quad;
            fn_802D511C(sprite, &quad, impostor, &right, &up);

            if (hasQuads)
            {
                for (int vertex = 0; vertex < 4; ++vertex)
                {
                    WriteVertex_802D4290(writer, quad, vertex, *impostor);
                }
            }
            else
            {
                for (int vertex = 0; vertex < 6; ++vertex)
                {
                    WriteVertex_802D4290(
                        writer, quad, lbl_8052E778[vertex], *impostor);
                }
            }
            ++rendered;
        }

        if (lbl_806E1F48)
        {
            texture = checkerTexture;
        }
        UnidentifiedTextureState* textureState =
            (UnidentifiedTextureState*)writer->model->packets->unknown20;
        textureState->texture = texture;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (fn_802A7E9C(writer))
        {
            target->AttachModel(writer->model, lbl_806DF414);
        }
        else
        {
            fn_8004F594(3, lbl_8052E7A0);
        }
    }
    else
    {
        fn_8004F594(3, lbl_8052E7C8);
    }

    if (skipCapture)
    {
        fn_802A7CB0(writer, 1);
    }
    return rendered;
}

extern "C" void fn_802D5034(ImpostorSprite_802D4290* sprite)
{
    sprite->mUnidentified084 = 0;
}

extern "C" void fn_802D5040(ImpostorSprite_802D4290* sprite)
{
    int i = 0;
    for (; i < sprite->mUnidentified05C; ++i)
    {
        sprite->mUnidentified080[i] = sprite->mUnidentified058[i];
    }
    sprite->mUnidentified084 = sprite->mUnidentified05C;
}

extern "C" void fn_802D5078(ImpostorSprite_802D4290* sprite, int slot)
{
    if (sprite->mUnidentified084 == sprite->mUnidentified060)
    {
        return;
    }
    sprite->mUnidentified080[sprite->mUnidentified084] = slot;
    ++sprite->mUnidentified084;
}

extern "C" unsigned long fn_802D50A4(ImpostorSprite_802D4290* sprite)
{
    return fn_802CDAA8(sprite->mUnidentified068->GetRenderPair());
}

extern "C" bool fn_802D50D8(
    ImpostorSprite_802D4290* sprite, int slot)
{
    if (sprite->mUnidentified05C == sprite->mUnidentified060)
    {
        return false;
    }
    sprite->mUnidentified058[sprite->mUnidentified05C] = slot;
    ++sprite->mUnidentified05C;
    return true;
}

extern "C" void fn_802D5110(ImpostorSprite_802D4290* sprite)
{
    sprite->mUnidentified05C = 0;
}

extern "C" void fn_802D511C(ImpostorSprite_802D4290* sprite,
    UnidentifiedImpostorQuad_802D511C* quad, const Impostor* impostor,
    const nlVector3* right, const nlVector3* up)
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

extern "C" int fn_802D536C(ImpostorSprite_802D4290* sprite)
{
    int sum = 0;
    for (int i = 0; i < sprite->mUnidentified084; ++i)
    {
        sum += sprite->mUnidentified080[i];
    }
    return sum;
}

void UnidentifiedImpostorView_802D4290::GetViewProjectionMatrix(
    nlMatrix4& matrix)
{
    UpdateMatrices();
    matrix = mViewProjection;
}

void UnidentifiedImpostorView_802D4290::GetInverseViewMatrix(
    nlMatrix4& matrix)
{
    UpdateMatrices();
    matrix = mInverseView;
}

void UnidentifiedImpostorView_802D4290::GetProjectionMatrix(
    nlMatrix4& matrix)
{
    matrix = mProjection;
}

const nlMatrix4*
UnidentifiedImpostorView_802D4290::GetProjectionMatrix() const
{
    return &mProjection;
}

void UnidentifiedImpostorView_802D4290::GetViewMatrix(nlMatrix4& matrix)
{
    matrix = mView;
}

const nlMatrix4* UnidentifiedImpostorView_802D4290::GetViewMatrix() const
{
    return &mView;
}
