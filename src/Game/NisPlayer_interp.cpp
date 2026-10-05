void NisPlayer::DoFunctionCall(unsigned int functionId)
{
    switch (functionId)
    {
    case 0:
    {
        bool triggerParam1 = m_SP[-1] != 0;
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_DEPTH_OF_FIELD, frame, "", "", &params);
        break;
    }
    case 1:
    {
        float frame = ((float*)m_SP)[-1];
        m_SP -= 1;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_SHOW_ELECTRIC_FENCE, frame, "", "", &params);
        break;
    }
    case 2:
    {
        unsigned long triggerParam1 = m_SP[-1];
        const char* target = (const char*)m_SP[-2];
        const char* name = (const char*)m_SP[-3];
        float frame = ((float*)m_SP)[-4];
        m_SP -= 4;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_EFFECT, frame, name, target, &params);
        break;
    }
    case 3:
    {
        unsigned long triggerParam2 = m_SP[-1];
        unsigned long triggerParam1 = m_SP[-2];
        float frame = ((float*)m_SP)[-3];
        m_SP -= 3;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        params.param2 = triggerParam2;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_PLAY_SOUND, frame, "", "", &params);
        break;
    }
    case 4:
    {
        const char* target = (const char*)m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        float frame = ((float*)m_SP)[-3];
        m_SP -= 3;

        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_RAISE_EVENT, frame, name, target, NULL);
        break;
    }
    case 5:
    {
        float value = ((float*)m_SP)[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.float1 = value;
        if (params.float1 > 1.0f)
        {
            params.float1 = 1.0f;
        }
        if (params.float1 < 0.0f)
        {
            params.float1 = 0.0f;
        }
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_CHARACTER_DIRT, frame, "", "", &params);
        break;
    }
    case 6:
    {
        bool triggerParam1 = m_SP[-1] != 0;
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_CROWD_EXCITEMENT, frame, "", "", &params);
        break;
    }
    case 7:
    {
        unsigned long triggerParam1 = m_SP[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_RUMBLE, frame, "", "", &params);
        break;
    }
    case 8:
    {
        unsigned long triggerParam1 = m_SP[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_STADIUM_EFFECTS, frame, "", "", &params);
        break;
    }
    case 9:
    {
        float frame = ((float*)m_SP)[-1];
        m_SP -= 1;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_HIDE_ELECTRIC_FENCE, frame, "", "", &params);
        break;
    }
    case 10:
    {
        float delta = ((float*)m_SP)[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.float1 = delta;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_TIME_DILATION, frame, "", "", &params);
        break;
    }
    default:
        nlBreak();
        break;
    }
}

