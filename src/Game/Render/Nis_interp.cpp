void Nis::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
    {
        NisPlayer* player = NisPlayer::Instance();
        player->mSuppressBlinking = true;
        break;
    }
    case 1:
    {
        int stadium = m_SP[-1];
        m_SP[-1] = GameInfoManager::Instance()->GetStadium() == stadium;
        if (m_RunState == 3)
        {
            m_SP[-1] = stadium;
        }
        break;
    }
    case 2:
    {
        bool force = m_SP[-1] != 0;
        NisWinnerType winnerType = (NisWinnerType)m_SP[-2];
        NisTarget target = (NisTarget)m_SP[-3];
        const char* proxyName = (const char*)m_SP[-4];
        const char* animName = (const char*)m_SP[-5];
        m_SP -= 5;
        PlayAnimProxy(animName, proxyName, target, winnerType, force);
        break;
    }
    case 3:
    {
        float fogEnd = *(float*)(m_SP - 1);
        float fogStart = *(float*)(m_SP - 2);
        m_SP -= 2;
        NisPlayer* player = NisPlayer::Instance();
        player->mRequestedFogStart = fogStart;
        player->mRequestedFogEnd = fogEnd;
        break;
    }
    default:
        nlBreak();
        break;
    }
}
