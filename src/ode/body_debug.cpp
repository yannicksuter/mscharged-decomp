#include "objects.h"

#include "Game/DebugWriteCache.h"

extern "C" void fn_802AADE8(void*, void*, unsigned int);

extern unsigned short s_dxBodyType;
extern char s_dxBodyName[7];
extern char s_BodyFlagsName[6];
extern char s_BodyGeomName[5];
extern char s_BodyMassMassName[10];
extern char s_BodyMassCName[7];
extern char s_BodyMassIName[7];
extern char s_BodyInvIName[5];
extern char s_BodyInvMassName[8];
extern char s_BodyPositionName[4];
extern char s_BodyQuaternionName[2];
extern char s_BodyRotationName[2];
extern char s_BodyLinearVelocityName[5];
extern char s_BodyAngularVelocityName[5];
extern char s_BodyForceAccumulatorName[5];
extern char s_BodyTorqueAccumulatorName[5];
extern char s_BodyFiniteRotationAxisName[16];
extern char s_BodyLinearThresholdName[22];
extern char s_BodyAngularThresholdName[23];
extern char s_BodyIdleTimeName[15];
extern char s_BodyIdleStepsName[16];
extern char s_BodyTimeLeftName[14];
extern char s_BodyStepsLeftName[15];

#define BODY_FIELD_OFFSET(body, field) \
    ((unsigned char*)&(body)->field - (unsigned char*)&(body)->flags)

extern "C" void fn_80358B08(dBodyID body, void* context, DebugWriteCache* cache)
{
    if (s_dxBodyType == 0xFFFF)
    {
        s_dxBodyType = fn_80338EBC(cache, s_dxBodyName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, 0, s_BodyFlagsName);
        fn_80338F88(
            cache, 15, lbl_80533C98[15].size, BODY_FIELD_OFFSET(body, geom), s_BodyGeomName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, mass.mass), s_BodyMassMassName);
        fn_80338F88(cache, 28, lbl_80533C98[28].size, BODY_FIELD_OFFSET(body, mass.c), s_BodyMassCName);
        fn_80338F88(cache, 30, lbl_80533C98[30].size, BODY_FIELD_OFFSET(body, mass.I), s_BodyMassIName);
        fn_80338F88(
            cache, 30, lbl_80533C98[30].size, BODY_FIELD_OFFSET(body, invI), s_BodyInvIName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, invMass), s_BodyInvMassName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, pos), s_BodyPositionName);
        fn_80338F88(cache, 29, lbl_80533C98[29].size, BODY_FIELD_OFFSET(body, q), s_BodyQuaternionName);
        fn_80338F88(cache, 30, lbl_80533C98[30].size, BODY_FIELD_OFFSET(body, R), s_BodyRotationName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, lvel), s_BodyLinearVelocityName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, avel), s_BodyAngularVelocityName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, facc), s_BodyForceAccumulatorName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, tacc), s_BodyTorqueAccumulatorName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, BODY_FIELD_OFFSET(body, finite_rot_axis), s_BodyFiniteRotationAxisName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, adis.linear_threshold), s_BodyLinearThresholdName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, adis.angular_threshold), s_BodyAngularThresholdName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, adis.idle_time), s_BodyIdleTimeName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, BODY_FIELD_OFFSET(body, adis.idle_steps), s_BodyIdleStepsName);
        fn_80338F88(cache, 17, lbl_80533C98[17].size, BODY_FIELD_OFFSET(body, adis_timeleft), s_BodyTimeLeftName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, BODY_FIELD_OFFSET(body, adis_stepsleft), s_BodyStepsLeftName);
        fn_80338F78(cache);
    }

    unsigned int size = (unsigned char*)body + sizeof(dxBody) - (unsigned char*)&body->flags;
    void* data = fn_8033930C(cache, s_dxBodyType, &body->flags, size);
    if (data != 0)
    {
        dxBody* copiedBody = (dxBody*)((unsigned char*)data - ((unsigned char*)&body->flags - (unsigned char*)body));
        copiedBody->geom = 0;
        fn_802AADE8(context, data, size);
    }
}

#undef BODY_FIELD_OFFSET

unsigned short s_dxBodyType = 0xFFFF;
char s_dxBodyName[7] = "dxBody";
char s_BodyFlagsName[6] = "flags";
char s_BodyGeomName[5] = "geom";
char s_BodyMassMassName[10] = "mass.mass";
char s_BodyMassCName[7] = "mass.c";
char s_BodyMassIName[7] = "mass.I";
char s_BodyInvIName[5] = "invI";
char s_BodyInvMassName[8] = "invMass";
char s_BodyPositionName[4] = "pos";
char s_BodyQuaternionName[2] = "q";
char s_BodyRotationName[2] = "R";
char s_BodyLinearVelocityName[5] = "lvel";
char s_BodyAngularVelocityName[5] = "avel";
char s_BodyForceAccumulatorName[5] = "facc";
char s_BodyTorqueAccumulatorName[5] = "tacc";
char s_BodyFiniteRotationAxisName[16] = "finite_rot_axis";
char s_BodyLinearThresholdName[22] = "adis.linear_threshold";
char s_BodyAngularThresholdName[23] = "adis.angular_threshold";
char s_BodyIdleTimeName[15] = "adis.idle_time";
char s_BodyIdleStepsName[16] = "adis.idle_steps";
char s_BodyTimeLeftName[14] = "adis_timeleft";
char s_BodyStepsLeftName[15] = "adis_stepsleft";
