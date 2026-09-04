#include "unclassified/tu_801AE530.h"

#include "Game/Render/RLView.h"

#include "Game/Event.h"
#include "Game/Game.h"
#include "Game/Team.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlAVLTree.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry_801AE530;

extern "C"
{
    UnidentifiedNumberModel_801AE530** fn_80276380();
    bool fn_80273B00();
    bool fn_802CC8FC(glModelPacket* packet, unsigned long hash);
    void fn_802CC3C8(glModelPacket* packet, unsigned long hash,
        const void* value, unsigned long count);

    extern UnidentifiedEventRegistry_801AE530* lbl_806E1D90;

    const float lbl_804DCF08[10] = {
        0.1662f,
        0.1164f,
        0.1682f,
        0.1658f,
        0.1749f,
        0.1675f,
        0.1667f,
        0.1644f,
        0.1632f,
        0.1671f,
    };
    const nlVector3 lbl_804DCF30 = { 1.0f, 0.0f, 0.0f };
    const nlVector3 lbl_804DCF3C = { 0.0f, 1.0f, 0.0f };
    const nlVector3 lbl_804DCF48 = { 0.0f, 0.0f, 1.0f };
    const nlVector4 lbl_804DCF58 = { 1.0f, 1.0f, 1.0f, 0.0f };

    float lbl_806DD040 = -0.0f;
    float lbl_806DD044 = 0.6f;
    float lbl_806DD048 = 1.0f;
    float lbl_806DD04C = 0.4f;
    float lbl_806DD050 = 0.4f;
    float lbl_806DD054 = 0.8f;
    float lbl_806DD058 = 1.05f;
    float lbl_806DD05C = 140.0f;
    float lbl_806DD060 = 50.0f;
    float lbl_806DD064 = 60.0f;
    float lbl_806DD068 = 82.0f;
    float lbl_806DD06C = 1.0f;
    float lbl_806DD070 = 650.0f;
    float lbl_806DD074 = 400.0f;
    float lbl_806DD078 = 500.0f;
    float lbl_806DD07C = 220.0f;
    float lbl_806DD080 = 80.0f;
    float lbl_806DD084 = 80.0f;
    float lbl_806DD088 = 0.45f;
    float lbl_806DD08C = 50.0f;
    float lbl_806DD090 = 800.0f;
    float lbl_806DD094 = 0.6f;
    float lbl_806DD098[2] = { 90.0f, 0.0f };

    UnidentifiedNumberDisplay_801AE530* gpNumberDisplay;
    bool lbl_806E162C;
    bool lbl_806E162D;
    float lbl_806E1630;
    float lbl_806E1634;
    float lbl_806E1638;
    float lbl_806E163C;
    float lbl_806E1640;
}

static inline float UnidentifiedLerp_801AE530(
    float first, float second, float amount)
{
    return (second - first) * amount + first;
}

UnidentifiedNumberDisplay_801AE530::UnidentifiedNumberDisplay_801AE530()
{
    mUnidentified004 = false;
    mUnidentified005 = false;
    mUnidentified010 = false;
    mUnidentified014 = 0.0f;
    mUnidentified018 = 0.0f;
    mUnidentified01C = 0.0f;
    mUnidentified020 = 0;
    mUnidentified024 = false;
    mUnidentified025 = false;

    mModels = fn_80276380();
    mUnidentified008 = 0;
    mUnidentified00C = 0;

    Function<void*> callback(
        Bind<void>(MemFun(&UnidentifiedNumberDisplay_801AE530::fn_801AE71C), this));
    unsigned int hash = fn_802B289C("GetReadyForKickoff", -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent<void>*)event)->Add(callback, 0, -1);
}

UnidentifiedNumberDisplay_801AE530::~UnidentifiedNumberDisplay_801AE530()
{
}

void UnidentifiedNumberDisplay_801AE530::fn_801AE71C()
{
    mUnidentified005 = false;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AE728(float deltaTime)
{
    if (g_pGame->mUnidentified040)
    {
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        mUnidentified025 = false;
    }

    if (mUnidentified024)
    {
        float amount = 1.0f;
        if (lbl_806DD048 != 0.0f)
        {
            amount = deltaTime / lbl_806DD048;
        }
        mUnidentified014 += amount;
        mUnidentified014
            = 1.0f <= mUnidentified014 ? 1.0f : mUnidentified014;
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 0x10
        && !mUnidentified025)
    {
        return;
    }

    if (mUnidentified010 || lbl_806E162C)
    {
        float amount = 1.0f;
        if (lbl_806E1634 != 0.0f)
        {
            amount = deltaTime / lbl_806E1634;
        }
        mUnidentified014 += amount;
        mUnidentified014
            = 1.0f <= mUnidentified014 ? 1.0f : mUnidentified014;
    }
    else
    {
        float amount = 1.0f;
        if (lbl_806DD044 != 0.0f)
        {
            amount = deltaTime / lbl_806DD044;
        }
        mUnidentified014 -= amount;
        mUnidentified014
            = 0.0f >= mUnidentified014 ? 0.0f : mUnidentified014;
    }

    if (mUnidentified018 > 0.0f)
    {
        mUnidentified018 -= deltaTime;
        if (mUnidentified018 <= 0.0f)
        {
            mUnidentified018 = 0.0f;
            bool changed = false;
            if (g_pTeams[0]->m_nScore > mUnidentified008)
            {
                ++mUnidentified008;
                changed = true;
            }
            if (g_pTeams[1]->m_nScore > mUnidentified00C)
            {
                ++mUnidentified00C;
                changed = true;
            }
            if (changed)
            {
                mUnidentified018
                    = mUnidentified025 ? lbl_806DD050 : lbl_806DD04C;
            }
        }
    }

    if (mUnidentified005)
    {
        return;
    }

    if (mUnidentified010 && mUnidentified018 == 0.0f)
    {
        mUnidentified01C -= deltaTime;
        if (mUnidentified01C <= 0.0f)
        {
            mUnidentified01C = 0.0f;
            mUnidentified010 = false;
        }
    }
    else if (!mUnidentified010)
    {
        mUnidentified01C = 0.0f;
    }
}

void UnidentifiedNumberDisplay_801AE530::Render()
{
    mModels = fn_80276380();
    if (mModels == 0)
    {
        return;
    }
    if (g_pGame->m_eGameState == 3
        && nlTaskManager::m_pInstance->mCurrentState == 8)
    {
        return;
    }
    if (!mUnidentified004)
    {
        return;
    }

    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 0x10 && !mUnidentified024 && !mUnidentified025)
    {
        return;
    }
    if (state == 1)
    {
        return;
    }

    if (mUnidentified024)
    {
        float scale = UnidentifiedLerp_801AE530(
            lbl_806DD08C, lbl_806DD090, mUnidentified014);
        nlVector2 position;
        position.x = 320.0f;
        position.y = 240.0f;
        fn_801AEE80(mUnidentified020, scale, lbl_806DD094, position);
    }
    else if (mUnidentified025 || state != 0x10)
    {
        fn_801AEA90();
    }
}

void UnidentifiedNumberDisplay_801AE530::fn_801AEA90()
{
    float positionY = UnidentifiedLerp_801AE530(
        lbl_806DD060, lbl_806DD07C, mUnidentified014);
    float separation = UnidentifiedLerp_801AE530(
        lbl_806DD064, lbl_806DD080, mUnidentified014);
    float scale = UnidentifiedLerp_801AE530(
        lbl_806DD05C, lbl_806DD070, mUnidentified014);
    float opacity = UnidentifiedLerp_801AE530(
        lbl_806DD06C, lbl_806DD088, mUnidentified014);

    if (fn_80273B00())
    {
        separation = UnidentifiedLerp_801AE530(
            lbl_806DD068, lbl_806DD084, mUnidentified014);
    }

    if (mUnidentified008 >= 100 || mUnidentified00C >= 100)
    {
        float endScale = fn_80273B00() ? lbl_806DD078 : lbl_806DD074;
        scale = UnidentifiedLerp_801AE530(
            lbl_806DD05C, endScale, mUnidentified014);
    }

    nlVector2 position;
    position.y = positionY + scale * 0.2f * 0.5f;
    float digitScale = fn_80273B00() ? scale * lbl_806DD054 : scale;

    int value = mUnidentified008;
    int remaining = value;
    float totalWidth = 0.0f;
    do
    {
        int digit = remaining % 10;
        totalWidth += digitScale * lbl_804DCF08[digit];
        if (remaining < 10)
        {
            break;
        }
        totalWidth += lbl_806DD040 * digitScale;
        remaining /= 10;
    } while (true);

    float offset = 0.0f;
    float leftEdge = 320.0f - separation;
    do
    {
        int digit = value % 10;
        float digitWidth = digitScale * lbl_804DCF08[digit];
        position.x = leftEdge - offset - digitWidth * 0.5f;
        fn_801AEE80(digit, scale, opacity, position);
        offset += digitWidth + lbl_806DD040 * digitScale;
        if (value >= 10)
        {
            value /= 10;
        }
        else
        {
            break;
        }
    } while (true);

    value = mUnidentified00C;
    remaining = value;
    totalWidth = 0.0f;
    do
    {
        int digit = remaining % 10;
        totalWidth += digitScale * lbl_804DCF08[digit];
        if (remaining < 10)
        {
            break;
        }
        totalWidth += lbl_806DD040 * digitScale;
        remaining /= 10;
    } while (true);

    float rightEdge = 320.0f + separation;
    do
    {
        int digit = value % 10;
        float digitWidth = digitScale * lbl_804DCF08[digit];
        position.x = totalWidth + rightEdge - digitWidth * 0.5f;
        fn_801AEE80(digit, scale, opacity, position);
        totalWidth -= digitWidth + lbl_806DD040 * digitScale;
        if (value >= 10)
        {
            value /= 10;
        }
        else
        {
            break;
        }
    } while (true);

    if (mUnidentified014 == 1.0f)
    {
        position.x = 320.0f;
        int separator = (g_pLocalization->m_CurrentLanguage
                             == nlLocalization::LangGerman
                         || lbl_806E162D)
                          ? 11
                          : 10;
        fn_801AEE80(separator, scale, opacity, position);
    }
}

void UnidentifiedNumberDisplay_801AE530::fn_801AEE80(
    int modelIndex, float scale, float opacity, const nlVector2& position)
{
    UnidentifiedNumberModel_801AE530* source = mModels[modelIndex];
    source->mUnidentified070 |= 1;

    nlMatrix4 transform;
    transform.SetIdentity();
    if (fn_80273B00())
    {
        nlMakeScaleMatrix(transform,
            scale * lbl_806DD054, scale, scale * lbl_806DD058);
    }
    else
    {
        nlMakeScaleMatrix(transform, scale, scale, scale);
    }

    if (lbl_806DD098[0] != 0.0f)
    {
        nlMatrix4 rotation;
        nlMatrix4 result;
        rotation.SetIdentity();
        nlMakeRotationMatrixAxisAngle(rotation, lbl_804DCF30,
            3.1415927f * lbl_806DD098[0] / 180.0f);
        nlMultMatrices(result, transform, rotation);
        transform = result;
    }
    if (lbl_806E163C != 0.0f)
    {
        nlMatrix4 rotation;
        nlMatrix4 result;
        rotation.SetIdentity();
        nlMakeRotationMatrixAxisAngle(rotation, lbl_804DCF3C,
            3.1415927f * lbl_806E163C / 180.0f);
        nlMultMatrices(result, transform, rotation);
        transform = result;
    }
    if (lbl_806E1640 != 0.0f)
    {
        nlMatrix4 rotation;
        nlMatrix4 result;
        rotation.SetIdentity();
        nlMakeRotationMatrixAxisAngle(rotation, lbl_804DCF48,
            3.1415927f * lbl_806E1640 / 180.0f);
        nlMultMatrices(result, transform, rotation);
        transform = result;
    }

    transform.m41 = position.x;
    transform.m42 = position.y;
    transform.m43 = lbl_806E1630;
    transform.m44 = 1.0f;

    glModel* model = glModelDupNoStreams(source->mModel, false, 0);
    glModelSetMatrix(model, transform);

    static unsigned long constantColourHash
        = nlStringLowerHash("constantcolour");
    nlVector4 colour = lbl_804DCF58;
    colour.w = opacity;
    glModelPacket* packet = model->packets;
    while (packet < model->packets + model->numPackets)
    {
        if (fn_802CC8FC(packet, constantColourHash))
        {
            fn_802CC3C8(packet, constantColourHash, &colour, 4);
        }
        glSetRasterState(packet->rasterState, GLS_Culling, 0);
        glSetRasterState(packet->rasterState, GLS_AlphaBlend, 1);
        glSetRasterState(packet->rasterState, GLS_AlphaTest, 1);
        glSetRasterState(packet->rasterState, GLS_AlphaTestRef, 3);
        ++packet;
    }
    GetLayerView(eCLV_UnsortedOrtho640)->AttachModel(model, 0);
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF250(
    int firstScore, int secondScore)
{
    mUnidentified008 = firstScore;
    mUnidentified00C = secondScore;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF25C()
{
    mUnidentified004 = false;
    mUnidentified005 = false;
    mUnidentified010 = false;
    mUnidentified014 = 0.0f;
    mUnidentified018 = 0.0f;
    mUnidentified01C = 0.0f;
    mUnidentified020 = 0;
    mUnidentified024 = false;
    mUnidentified025 = false;
    mUnidentified008 = 0;
    mUnidentified00C = 0;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF294()
{
    mUnidentified010 = true;
    mUnidentified018 = lbl_806DD04C;
    mUnidentified005 = true;
    mUnidentified01C = lbl_806E1638;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF2B4()
{
    mUnidentified004 = true;
    mUnidentified014 = 0.0f;
    ++mUnidentified020;
    mUnidentified024 = true;
    mUnidentified025 = false;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF2E0()
{
    mUnidentified020 = 0;
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF2EC()
{
    mUnidentified004 = true;
    if (mUnidentified024 == true)
    {
        mUnidentified024 = false;
    }
}

void UnidentifiedNumberDisplay_801AE530::fn_801AF30C()
{
    mUnidentified004 = true;
    mUnidentified025 = true;
    if (mUnidentified020 > 0)
    {
        mUnidentified010 = true;
        mUnidentified020 = 0;
        mUnidentified018 = lbl_806DD04C;
        mUnidentified005 = true;
        mUnidentified01C = lbl_806E1638;
    }
}
