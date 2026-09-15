#include "objects.h"

#include "Game/DebugWriteCache.h"
#include "NL/nlMain.h"
#include "ode/NLGAdditions.h"

static unsigned short s_dxBodyType = 0xFFFF;

#define BODY_FIELD_OFFSET(body, field) \
    ((unsigned char*)&(body)->field - (unsigned char*)&(body)->flags)

void dBodySyncLog(dBodyID body, void* context, DebugWriteCache* cache)
{
    if (s_dxBodyType == 0xFFFF)
    {
        s_dxBodyType = cache->BeginType("dxBody");
        cache->AddField("flags", 8, 0);
        cache->AddField("geom", 15, BODY_FIELD_OFFSET(body, geom));
        cache->AddField("mass.mass", 17, BODY_FIELD_OFFSET(body, mass.mass));
        cache->AddField("mass.c", 28, BODY_FIELD_OFFSET(body, mass.c));
        cache->AddField("mass.I", 30, BODY_FIELD_OFFSET(body, mass.I));
        cache->AddField("invI", 30, BODY_FIELD_OFFSET(body, invI));
        cache->AddField("invMass", 17, BODY_FIELD_OFFSET(body, invMass));
        cache->AddField("pos", 27, BODY_FIELD_OFFSET(body, pos));
        cache->AddField("q", 29, BODY_FIELD_OFFSET(body, q));
        cache->AddField("R", 30, BODY_FIELD_OFFSET(body, R));
        cache->AddField("lvel", 27, BODY_FIELD_OFFSET(body, lvel));
        cache->AddField("avel", 27, BODY_FIELD_OFFSET(body, avel));
        cache->AddField("facc", 27, BODY_FIELD_OFFSET(body, facc));
        cache->AddField("tacc", 27, BODY_FIELD_OFFSET(body, tacc));
        cache->AddField("finite_rot_axis", 27, BODY_FIELD_OFFSET(body, finite_rot_axis));
        cache->AddField("adis.linear_threshold", 17, BODY_FIELD_OFFSET(body, adis.linear_threshold));
        cache->AddField("adis.angular_threshold", 17, BODY_FIELD_OFFSET(body, adis.angular_threshold));
        cache->AddField("adis.idle_time", 17, BODY_FIELD_OFFSET(body, adis.idle_time));
        cache->AddField("adis.idle_steps", 8, BODY_FIELD_OFFSET(body, adis.idle_steps));
        cache->AddField("adis_timeleft", 17, BODY_FIELD_OFFSET(body, adis_timeleft));
        cache->AddField("adis_stepsleft", 8, BODY_FIELD_OFFSET(body, adis_stepsleft));
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
