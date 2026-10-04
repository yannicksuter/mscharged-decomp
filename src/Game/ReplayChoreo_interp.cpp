void ReplayChoreo::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
        ++m_SP;
        m_SP[-1] = mCurrentHighlight == 0 ? -1 : GetHighlightNumber();
        break;
    case 1:
        m_SP--;
        break;
    case 2:
        m_SP--;
        break;
    case 3:
        m_SP--;
        break;
    case 4:
        m_SP -= 2;
        break;
    case 5:
        mCamera.mFrozen = true;
        break;
    case 6:
        mCamera.mPositionFrozen = true;
        break;
    case 7:
        ++m_SP;
        *(float*)&m_SP[-1] = mCamera.mFov;
        break;
    case 8:
    {
        int original = m_SP[-2];
        m_SP--;
        m_SP[-1] = PickRandom(original);
        if (m_RunState == 3)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case 9:
        ++m_SP;
        m_SP[-1] = GameInfoManager::Instance()->GetStadium();
        break;
    case 10:
        ++m_SP;
        m_SP[-1] = mIsHighlightReel;
        break;
    case 11:
    {
        ReplayCameraPosition position = (ReplayCameraPosition)m_SP[-1];
        m_SP--;
        mCamera.MoveTo(position);
        break;
    }
    case 12:
    {
        float maxHeight = *(float*)&m_SP[-1];
        float maxBeyondSideLine = *(float*)&m_SP[-2];
        float maxBehindGoalLine = *(float*)&m_SP[-3];
        m_SP -= 3;
        mCamera.SetPositionLimits(maxBehindGoalLine, maxBeyondSideLine, maxHeight);
        break;
    }
    case 13:
    {
        float kickoffTime;
        float resetTime;
        float earliest;
        float time;
        float timeOffset = *(float*)&m_SP[-1];
        unsigned int event = m_SP[-2];
        m_SP -= 2;
        time = timeOffset + mReplay->TimeOfLastOccurence(event);
        earliest = mReplayManager->mReplay->EndTime() - 30.0f;
        kickoffTime = mReplay->TimeOfLastOccurence(0x20);
        resetTime = mReplay->TimeOfLastOccurence(0x40);
        time = time >= kickoffTime ? time : kickoffTime;
        time = time >= resetTime ? time : resetTime;
        time = time >= earliest ? time : earliest;
        mReplayManager->SetCurrentTime(time);
        break;
    }
    case 14:
    {
        float kd = *(float*)&m_SP[-1];
        float ks = *(float*)&m_SP[-2];
        float y = *(float*)&m_SP[-3];
        float x = *(float*)&m_SP[-4];
        m_SP -= 4;
        mRumbleFilter.Rumble(x, y, ks, kd);
        break;
    }
    case 15:
    {
        float time = *(float*)&m_SP[-1];
        m_SP--;
        if (mRunningFor && 0.0f >= mRunForTimeLeft)
        {
            mRunForTimeLeft = 0.0f;
            mRunningFor = false;
            break;
        }
        if (!mRunningFor)
        {
            mRunForTimeLeft = time;
            mRunningFor = true;
        }
        StopWithUndo();
        break;
    }
    case 16:
    {
        float currentTime;
        float timeOffset = *(float*)&m_SP[-1];
        unsigned int event = m_SP[-2];
        m_SP -= 2;
        if (!IsFinished())
        {
            currentTime = mReplayManager->mTime;
            if (currentTime < timeOffset + mReplay->TimeOfLastOccurence(event))
            {
                StopWithUndo();
            }
        }
        break;
    }
    case 17:
    {
        float degrees = *(float*)&m_SP[-1];
        m_SP--;
        mCamera.mBallToGoalRotationDegrees = degrees;
        break;
    }
    case 18:
    {
        ReplayCameraPosition position = (ReplayCameraPosition)m_SP[-1];
        m_SP--;
        mCamera.CutTo(position);
        mCamera.mNoDampenForOneUpdate = true;
        break;
    }
    case 19:
    {
        int focus = m_SP[-1];
        m_SP--;
        mCamera.mFocus = focus;
        mCamera.mSecondaryFocus = focus;
        break;
    }
    case 20:
    {
        float fov = *(float*)&m_SP[-1];
        m_SP--;
        mCamera.mFov = fov;
        mCamera.mDeltaFov = 0.0f;
        mCamera.mAutoFov = false;
        break;
    }
    case 21:
    {
        float z = *(float*)&m_SP[-1];
        float y = *(float*)&m_SP[-2];
        float x = *(float*)&m_SP[-3];
        m_SP -= 3;
        nlVector3 offset;
        offset.x = x;
        offset.y = y;
        offset.z = z;
        mCamera.SetPositionOffset(offset);
        break;
    }
    case 22:
    {
        float dampingTime = *(float*)&m_SP[-1];
        m_SP--;
        // The cast passes a temporary, stored after the pop as in retail.
        mCamera.SetPositionDampingTime((float)dampingTime);
        break;
    }
    case 23:
    {
        float dampingTime = *(float*)&m_SP[-1];
        m_SP--;
        mCamera.SetLookAtDampingTime((float)dampingTime);
        break;
    }
    case 24:
    {
        int focus = m_SP[-1];
        m_SP--;
        mCamera.mSecondaryFocus = focus;
        break;
    }
    case 25:
    {
        float original = *(float*)&m_SP[-3];
        m_SP -= 2;
        float shotTime = mReplay->TimeOfLastOccurence(1);
        float passTime = mReplay->TimeOfLastOccurence(2);
        int result;
        if (shotTime != mReplay->BeginTime() && passTime != mReplay->BeginTime())
        {
            float speed = (shotTime - passTime) / original;
            if (speed > 0.6f)
            {
                speed = 0.6f;
            }
            if (speed < 0.3f)
            {
                speed = 0.3f;
            }
            mReplayManager->mSpeed = speed;
            mReplayManager->mSpeedUp = 0.0f;
            result = 1;
        }
        else
        {
            result = 0;
        }
        m_SP[-1] = result;
        if (m_RunState == 3)
        {
            *(float*)&m_SP[-1] = original;
        }
        break;
    }
    case 26:
    {
        float speed = *(float*)&m_SP[-1];
        m_SP--;
        mReplayManager->mSpeed = speed;
        mReplayManager->mSpeedUp = 0.0f;
        break;
    }
    case 27:
    {
        float maxChangeRate = *(float*)&m_SP[-1];
        float maxDistance = *(float*)&m_SP[-2];
        float minDistance = *(float*)&m_SP[-3];
        float maxFov = *(float*)&m_SP[-4];
        float minFov = *(float*)&m_SP[-5];
        m_SP -= 5;
        mCamera.SetAutoFov(minFov, maxFov, minDistance, maxDistance, maxChangeRate);
        break;
    }
    case 28:
    {
        float deltaFov = *(float*)&m_SP[-2];
        float targetFov = *(float*)&m_SP[-1];
        m_SP -= 2;
        mCamera.mTargetFov = targetFov;
        mCamera.mAutoFov = false;
        mCamera.mDeltaFov = (float)fabs(deltaFov);
        break;
    }
    case 29:
    {
        float speedUp = *(float*)&m_SP[-1];
        m_SP--;
        mReplayManager->mSpeedUp = speedUp;
        break;
    }
    case 30:
        mCamera.mFrozen = true;
        if (ReplayManager::Instance()->IsLoadingReplay() == true)
        {
            StopWithUndo();
        }
        break;
    case 31:
        ++m_SP;
        m_SP[-1] = !mIsHighlightReel ? NisPlayer::Instance()->mWinnerSide[1] == 0 : false;
        break;
    default:
        nlBreak();
        break;
    }
}
