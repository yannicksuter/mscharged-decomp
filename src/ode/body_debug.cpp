#include "objects.h"

#include "Game/DebugWriteCache.h"
#include "NL/nlMain.h"
#include "ode/NLGAdditions.h"

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

void dBodySyncLog(dBodyID body, void* context, DebugWriteCache* cache)
{
    if (s_dxBodyType == 0xFFFF)
    {
        s_dxBodyType = cache->BeginType(s_dxBodyName);
        cache->AddField(8, gDebugFieldTypes[8].size, 0, s_BodyFlagsName);
        cache->AddField(15, gDebugFieldTypes[15].size, BODY_FIELD_OFFSET(body, geom), s_BodyGeomName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, mass.mass), s_BodyMassMassName);
        cache->AddField(28, gDebugFieldTypes[28].size, BODY_FIELD_OFFSET(body, mass.c), s_BodyMassCName);
        cache->AddField(30, gDebugFieldTypes[30].size, BODY_FIELD_OFFSET(body, mass.I), s_BodyMassIName);
        cache->AddField(30, gDebugFieldTypes[30].size, BODY_FIELD_OFFSET(body, invI), s_BodyInvIName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, invMass), s_BodyInvMassName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, pos), s_BodyPositionName);
        cache->AddField(29, gDebugFieldTypes[29].size, BODY_FIELD_OFFSET(body, q), s_BodyQuaternionName);
        cache->AddField(30, gDebugFieldTypes[30].size, BODY_FIELD_OFFSET(body, R), s_BodyRotationName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, lvel), s_BodyLinearVelocityName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, avel), s_BodyAngularVelocityName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, facc), s_BodyForceAccumulatorName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, tacc), s_BodyTorqueAccumulatorName);
        cache->AddField(27, gDebugFieldTypes[27].size, BODY_FIELD_OFFSET(body, finite_rot_axis), s_BodyFiniteRotationAxisName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, adis.linear_threshold), s_BodyLinearThresholdName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, adis.angular_threshold), s_BodyAngularThresholdName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, adis.idle_time), s_BodyIdleTimeName);
        cache->AddField(8, gDebugFieldTypes[8].size, BODY_FIELD_OFFSET(body, adis.idle_steps), s_BodyIdleStepsName);
        cache->AddField(17, gDebugFieldTypes[17].size, BODY_FIELD_OFFSET(body, adis_timeleft), s_BodyTimeLeftName);
        cache->AddField(8, gDebugFieldTypes[8].size, BODY_FIELD_OFFSET(body, adis_stepsleft), s_BodyStepsLeftName);
        cache->EndType();
    }

    unsigned int size = (unsigned char*)body + sizeof(dxBody) - (unsigned char*)&body->flags;
    void* data = cache->WriteData(s_dxBodyType, &body->flags, size);
    if (data != 0)
    {
        dxBody* copiedBody = (dxBody*)((unsigned char*)data - ((unsigned char*)&body->flags - (unsigned char*)body));
        copiedBody->geom = 0;
        static_cast<RunningChecksum*>(context)->ChecksumData(data, size);
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
