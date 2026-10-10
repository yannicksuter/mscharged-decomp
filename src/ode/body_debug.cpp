#include "objects.h"

#include "Game/DebugWriteCache.h"
#include "NL/nlMain.h"
#include "ode/NLGAdditions.h"

static unsigned short s_dxBodyType = DEBUG_TYPE_NONE;

#define BODY_FIELD_OFFSET(body, field) \
    ((unsigned char*)&(body)->field - (unsigned char*)&(body)->flags)

void dBodySyncLog(dBodyID body, void* context, DebugWriteCache* cache)
{
    if (s_dxBodyType == DEBUG_TYPE_NONE)
    {
        s_dxBodyType = cache->BeginType("dxBody");
        cache->AddField("flags", DEBUG_FIELD_INT, 0);
        cache->AddField("geom", DEBUG_FIELD_POINTER, BODY_FIELD_OFFSET(body, geom));
        cache->AddField("mass.mass", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, mass.mass));
        cache->AddField("mass.c", DEBUG_FIELD_ODE_VECTOR4, BODY_FIELD_OFFSET(body, mass.c));
        cache->AddField("mass.I", DEBUG_FIELD_ODE_MATRIX3, BODY_FIELD_OFFSET(body, mass.I));
        cache->AddField("invI", DEBUG_FIELD_ODE_MATRIX3, BODY_FIELD_OFFSET(body, invI));
        cache->AddField("invMass", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, invMass));
        cache->AddField("pos", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, pos));
        cache->AddField("q", DEBUG_FIELD_ODE_QUATERNION, BODY_FIELD_OFFSET(body, q));
        cache->AddField("R", DEBUG_FIELD_ODE_MATRIX3, BODY_FIELD_OFFSET(body, R));
        cache->AddField("lvel", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, lvel));
        cache->AddField("avel", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, avel));
        cache->AddField("facc", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, facc));
        cache->AddField("tacc", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, tacc));
        cache->AddField("finite_rot_axis", DEBUG_FIELD_ODE_VECTOR3, BODY_FIELD_OFFSET(body, finite_rot_axis));
        cache->AddField("adis.linear_threshold", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, adis.linear_threshold));
        cache->AddField("adis.angular_threshold", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, adis.angular_threshold));
        cache->AddField("adis.idle_time", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, adis.idle_time));
        cache->AddField("adis.idle_steps", DEBUG_FIELD_INT, BODY_FIELD_OFFSET(body, adis.idle_steps));
        cache->AddField("adis_timeleft", DEBUG_FIELD_FLOAT, BODY_FIELD_OFFSET(body, adis_timeleft));
        cache->AddField("adis_stepsleft", DEBUG_FIELD_INT, BODY_FIELD_OFFSET(body, adis_stepsleft));
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
