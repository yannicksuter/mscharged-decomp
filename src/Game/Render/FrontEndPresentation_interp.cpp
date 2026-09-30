void FrontEndPresentation::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
        NetTournManager::Instance()->DetachTournamentTrophy();
        break;
    case 1:
    {
        bool value = m_SP[-1] != 0;
        int side = (int)m_SP[-2];
        unsigned int name = m_SP[-3];
        m_SP -= 3;
        bool alternate = false;
        if (side == -1 && GameInfoManager::Instance()->mCurrentMode == 3)
        {
            side = g_pCupManager->mPendingCupTeam;
        }
        else
        {
            int captain = GameInfoManager::Instance()->GetTeam((short)side);
            int opponent = GameInfoManager::Instance()->GetTeam((short)!side);
            alternate = CaptainsNeedAlternateColour(captain, opponent);
            side = captain;
        }
        FEModelManager::Instance()->CreateModel(FE_MODEL_IMPOSTOR, (const char*)name, side, value, 0, 0, alternate);
        break;
    }
    case 2:
        FEModelManager::Instance()->ReleaseImpostors();
        break;
    case 3:
        NetTournManager::Instance()->DestroyTournamentTrophy();
        break;
    case 4:
        g_pCupManager->ShowRoundNews();
        break;
    case 5:
    {
        unsigned int value = m_SP[-1];
        --m_SP;
        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        if (stadium != 0)
        {
            stadium->TriggerEffects(value);
        }
        break;
    }
    case 6:
    {
        unsigned int value = m_SP[-1];
        m_SP[-1] = nlStringHash((const char*)value);
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 7:
        SetCupTrophiesVisible(false);
        break;
    case 8:
        StopWithUndo();
        break;
    case 9:
        ++m_SP;
        m_SP[-1] = g_e3_Build;
        break;
    case 10:
    {
        const char* animationName;
        FEModelHandle* object;
        const char* objectName;
        objectName = (const char*)m_SP[-2];
        animationName = (const char*)m_SP[-1];
        --m_SP;
        object = FEModelManager::Instance()->GetModel(objectName);
        unsigned int isPlaying;
        if (object != 0 && object->IsPlayingAnimation(animationName)
            && !object->IsAnimationFinished())
        {
            isPlaying = 1;
        }
        else
        {
            isPlaying = 0;
        }
        m_SP[-1] = isPlaying;
        if (m_RunState == 3)
        {
            m_SP[-1] = (unsigned int)objectName;
        }
        break;
    }
    case 11:
        ++m_SP;
        m_SP[-1] = IsWidescreen();
        break;
    case 12:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        DestroyFrontEndPresentationEmission(name);
        break;
    }
    case 13:
        BeginLoadTournamentTrophy();
        break;
    case 14:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        DisableFrontEndPresentationEmission(name);
        break;
    }
    case 15:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->SelectCameraAnimation(name);
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        break;
    }
    case 16:
    {
        ePlayMode playMode = (ePlayMode)m_SP[-1];
        const char* animationName = (const char*)m_SP[-2];
        const char* objectName = (const char*)m_SP[-3];
        m_SP -= 3;
        SetWorldAnimation(objectName, animationName, playMode);
        break;
    }
    case 17:
    {
        float value = *(float*)&m_SP[-1];
        unsigned int argument1 = m_SP[-2];
        const char* argument0 = (const char*)m_SP[-3];
        const char* objectName = (const char*)m_SP[-4];
        m_SP -= 4;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(objectName);
        if (object != 0)
        {
            object->PlayAnimation(argument0, (ePlayMode)argument1, 0.0f, value, false);
        }
        break;
    }
    case 18:
    {
        unsigned int argument1 = m_SP[-1];
        const char* argument0 = (const char*)m_SP[-2];
        const char* objectName = (const char*)m_SP[-3];
        m_SP -= 3;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(objectName);
        if (object != 0)
        {
            object->PlayAnimation(argument0, (ePlayMode)argument1, 0.0f, 0.0f, false);
        }
        break;
    }
    case 19:
        FEAudio::PlayAnimAudioEvent(0xB60A9CC0, 0, 0, true);
        break;
    case 20:
    {
        unsigned int event = m_SP[-1];
        --m_SP;
        FEAudio::PlayAnimAudioEvent(event, 0, 0, true);
        break;
    }
    case 21:
    {
        float duration = *(float*)&m_SP[-1];
        --m_SP;
        PopPresentationCamera(OnCameraTransitionFinished, duration);
        mCameraTransitionFinished = false;
        break;
    }
    case 22:
    {
        unsigned int childName = m_SP[-1];
        unsigned int objectName = m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel((const char*)objectName);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)FEModelManager::Instance()->GetObject(childName);
            if (child != 0)
            {
                object->SetTransform(*(nlMatrix4*)child->GetValue());
            }
        }
        break;
    }
    case 23:
    {
        bool value = m_SP[-1] != 0;
        float duration = *(float*)&m_SP[-2];
        const char* name = (const char*)m_SP[-3];
        m_SP -= 3;
        PushPresentationCamera(name, OnCameraTransitionFinished, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        mCameraFinished = false;
        mCameraTransitionFinished = false;
        break;
    }
    case 24:
    {
        bool value = m_SP[-1] != 0;
        float duration = *(float*)&m_SP[-2];
        const char* baseName = (const char*)m_SP[-3];
        m_SP -= 3;
        char name[64];
        int mode = g_pCupManager->GetCurrentMode();
        if (mode == 0)
            nlSNPrintf(name, sizeof(name), sBronzeFormat, baseName);
        else if (mode == 1)
            nlSNPrintf(name, sizeof(name), sSilverFormat, baseName);
        else
            nlSNPrintf(name, sizeof(name), sGoldFormat, baseName);
        PushPresentationCamera(name, OnCameraTransitionFinished, duration, value);
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        camera->m_bCyclic = false;
        camera->m_EndOfAnimationCallback = OnCameraAnimationFinished;
        mCameraFinished = false;
        mCameraTransitionFinished = false;
        break;
    }
    case 25:
    {
        int value1 = (int)m_SP[-1];
        int value0 = (int)m_SP[-2];
        m_SP -= 2;
        GameSceneManager::Instance()->Push((SceneList)value0, (ScreenMovement)value1, false);
        break;
    }
    case 26:
        GameSceneManager::Instance()->Push((SceneList)8, SCREEN_FORWARD, false);
        break;
    case 27:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            FEModelManager::Instance()->DestroyModel(object);
        }
        break;
    }
    case 28:
    {
        float time = *(float*)&m_SP[-1];
        --m_SP;
        GetCurrentAnimatedCamera()->SetAnimationTime(time, true);
        break;
    }
    case 29:
    {
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        float time = camera->GetUnidentifiedDuration();
        camera->SetAnimationTime(time, true);
        break;
    }
    case 30:
    {
        unsigned int value = m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->SetDefaultAnimation((const char*)value);
        }
        break;
    }
    case 31:
    {
        unsigned int childName = m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            PresentationLookupResult* child = (PresentationLookupResult*)FEModelManager::Instance()->GetObject(childName);
            if (child != 0)
            {
                object->SetPosition(*(nlVector3*)((char*)child->GetValue() + 0x30));
            }
        }
        break;
    }
    case 32:
    {
        float time = *(float*)&m_SP[-1];
        --m_SP;
        mWaitTime = time;
        break;
    }
    case 33:
    {
        bool value = m_SP[-1] != 0;
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->mEnabled = value;
        }
        break;
    }
    case 34:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            object->SetAnimationCompleteCallback(OnFrontEndPresentationModelAnimationFinished);
        }
        break;
    }
    case 35:
    {
        bool cyclic = m_SP[-1] != 0;
        --m_SP;
        cAnimCamera* camera = GetCurrentAnimatedCamera();
        if (camera != 0)
        {
            camera->m_bCyclic = cyclic;
        }
        break;
    }
    case 36:
        SetCupTrophiesVisible(true);
        break;
    case 37:
        ShowCupAwardRewardsPopup();
        break;
    case 38:
        ShowCupBrickWallNews();
        break;
    case 39:
        ShowCupRulesPopup();
        break;
    case 40:
        ShowCupGoldenBootNews();
        break;
    case 41:
        if (SHNavigation* scene = GetNavigationScene())
        {
            scene->StartTransition();
        }
        break;
    case 42:
        StartFrontEndPresentationMusic();
        break;
    case 43:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        EnableFrontEndPresentationEmission(name);
        break;
    }
    case 44:
        if (!mCameraFinished)
            StopWithUndo();
        else
            mCameraFinished = false;
        break;
    case 45:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0 && !object->IsAnimationFinished())
        {
            StopWithUndo();
        }
        break;
    }
    case 46:
        if (!mCameraTransitionFinished)
            StopWithUndo();
        else
            mCameraTransitionFinished = false;
        break;
    case 47:
        if (SaveEnabled && InOperation)
        {
            StopWithUndo();
        }
        break;
    case 48:
        mWaitTime -= mDeltaTime;
        if (mWaitTime > 0.0f)
            StopWithUndo();
        else
            mWaitTime = 0.0f;
        break;
    case 49:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        if (FEModelManager::Instance()->GetModel(name) == 0)
        {
            StopWithUndo();
        }
        break;
    }
    case 50:
    {
        const char* name = (const char*)m_SP[-1];
        --m_SP;
        FEModelHandle* object = FEModelManager::Instance()->GetModel(name);
        if (object != 0)
        {
            if (!object->IsLoaded())
            {
                StopWithUndo();
            }
        }
        else
        {
            StopWithUndo();
        }
        break;
    }
    case 51:
        if (IsTournamentTrophyLoaded())
            FinishLoadTournamentTrophy();
        else
            StopWithUndo();
        break;
    default:
        nlBreak();
        break;
    }
}

