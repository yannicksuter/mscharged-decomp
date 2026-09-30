extern "C" unsigned long FuzzyAIStringHash(const char* value)
{
    return nlStringHash(value);
}

extern "C" float fn_800E3FE0()
{
    return fn_800DFF1C();
}

extern "C" float fn_800E3FE4()
{
    return fn_800DFF60();
}

extern "C" float fn_800E3FE8()
{
    return fn_800E00F8();
}

extern "C" float fn_800E3FEC()
{
    return fn_800E0034();
}

void FuzzyAIRuntime::DoFunctionCall(unsigned int function)
{
    switch (function)
    {
    case 0:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = AbleToInterceptBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 1:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D82C0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 2:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7AB8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 3:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7B00(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 4:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D85F8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 5:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D8764(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 6:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D84F8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 7:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D7988(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 8:
    {
        bool arg3 = m_SP[-1] != 0;
        const char* arg2 = (const char*)m_SP[-2];
        int arg1 = (int)m_SP[-3];
        UnidentifiedScriptMachine* arg0 = (UnidentifiedScriptMachine*)m_SP[-4];
        m_SP -= 4;
        fn_80318D34(arg0, arg1, arg2, arg3);
        break;
    }
    case 9:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = AggressiveT(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 10:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA330(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 11:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA310(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 12:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD9C8(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 13:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = PlayerShotDistance(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 14:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DDD70(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 15:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D96F4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 16:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BF4(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 17:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C04(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 18:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C0C(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 19:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BE4(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 20:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIGetIteratorFielder_800E3F10(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 21:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIGetIteratorFielder_800E3F1C(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 22:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIGetIteratorAIContext(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 23:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BFC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 24:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3BEC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 25:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C5C(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 26:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C44(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 27:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E7ECC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 28:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7ED4(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 29:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C14(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 30:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E3C2C(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 31:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E8CB0(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 32:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7EE4(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 33:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E7EEC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 34:
    {
        Variant* arg0 = (Variant*)m_SP[-1];
        m_SP[-1] = (u32)fn_800E7EDC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 35:
    {
        FuzzyRuntimeBase* arg0 = (FuzzyRuntimeBase*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyGetCurrentContext(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 36:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9B0C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 37:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9A38(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 38:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9B74(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 39:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9BDC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 40:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9C24(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 41:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E06F4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 42:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF838(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 43:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = BallOwner(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 44:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = BallOwnerT(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 45:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D671C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 46:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Captain(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 47:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6708(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 48:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D795C(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 49:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF590(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 50:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DED80(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 51:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_80314494(arg0, arg1, arg2);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 52:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D674C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 53:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8A9C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 54:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8BAC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 55:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = CloseTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 56:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 57:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD45C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 58:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD504(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 59:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToFormationPosition(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 60:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToMyGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 61:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToMyNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 62:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D92DC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 63:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToSideline(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 64:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD2F4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 65:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D912C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 66:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE804(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 67:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToTheirGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 68:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = CloseToTheirNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 69:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = ClosingTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 70:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = ClosingTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 71:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        ((float*)m_SP)[-1] = FuzzyAIGetConfidence(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 72:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA130(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 73:
    {
        float arg0 = ((float*)m_SP)[-1];
        m_SP -= 1;
        FuzzyPrintFloat(arg0);
        break;
    }
    case 74:
    {
        const char* arg0 = (const char*)m_SP[-1];
        m_SP -= 1;
        FuzzyPrintString(this, arg0);
        break;
    }
    case 75:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Defence(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 76:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Defensive(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 77:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7910(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 78:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Difficult(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 79:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)FuzzyHasContextParameter(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 80:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = DoingS2S(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 81:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE4B0(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 82:
    {
        bool arg0 = m_SP[-1] != 0;
        m_SP -= 1;
        fn_80314740(this, arg0);
        break;
    }
    case 83:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE1F0(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 84:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DDF54(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 85:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE0A8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 86:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD744(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 87:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = FallenDown(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 88:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA050(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 89:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD4CC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 90:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD684(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 91:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD294(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 92:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD37C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 93:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = FarTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 94:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 95:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToFormationPosition(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 96:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToMyGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 97:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToMyNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 98:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D93F4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 99:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D924C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 100:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE994(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 101:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToTheirGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 102:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FarToTheirNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 103:
    {
        bool arg0 = m_SP[-1] != 0;
        ((float*)m_SP)[-1] = FuzzyAIBoolToFloat(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 104:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP -= 1;
        FuzzyAIDestroyFielderIterator(this, arg0);
        break;
    }
    case 105:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIAdvanceFielderIterator(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 106:
    {
        FuzzyFielderIterator* arg0 = (FuzzyFielderIterator*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIHasNextFielder(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 107:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAICreateTeamFielderIterator(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 108:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = FielderType(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 109:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7A70(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 110:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9FC8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 111:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        bool arg0 = m_SP[-3] != 0;
        m_SP -= 3;
        FuzzySetBoolParameter(this, arg0, arg1, arg2);
        break;
    }
    case 112:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cBall* arg0 = (cBall*)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetBallParameter(this, arg0, arg1, arg2);
        break;
    }
    case 113:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cPlayer* arg0 = (cPlayer*)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetPlayerParameter_800E8CB8(this, arg0, arg1, arg2);
        break;
    }
    case 114:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        const char* arg0 = (const char*)m_SP[-3];
        m_SP -= 3;
        FuzzySetStringParameter(this, arg0, arg1, arg2);
        break;
    }
    case 115:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        cPlayer* arg0 = (cPlayer*)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetPlayerParameter_800E3A84(this, arg0, arg1, arg2);
        break;
    }
    case 116:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetIntParameter_800E8D68(this, arg0, arg1, arg2);
        break;
    }
    case 117:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetIntParameter_800E8D6C(this, arg0, arg1, arg2);
        break;
    }
    case 118:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetIntParameter_800E8D70(this, arg0, arg1, arg2);
        break;
    }
    case 119:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetIntParameter_800E8D74(this, arg0, arg1, arg2);
        break;
    }
    case 120:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        FuzzyFielderReference* arg0 = (FuzzyFielderReference*)m_SP[-3];
        m_SP -= 3;
        FuzzyAISetFielderParameter(this, arg0, arg1, arg2);
        break;
    }
    case 121:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 3;
        FuzzySetFloatParameter(this, arg0, arg1, arg2);
        break;
    }
    case 122:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        int arg0 = (int)m_SP[-3];
        m_SP -= 3;
        FuzzySetIntParameter(this, arg0, arg1, arg2);
        break;
    }
    case 123:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        Variant& arg0 = *(Variant*)m_SP[-3];
        m_SP -= 3;
        FuzzySetVariantParameter(this, arg0, arg1, arg2);
        break;
    }
    case 124:
    {
        UnidentifiedVariant_80054AB8* arg2 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        unsigned long arg1 = (unsigned long)m_SP[-2];
        unsigned long arg0 = (unsigned long)m_SP[-3];
        m_SP -= 3;
        FuzzySetU32Parameter(this, arg0, arg1, arg2);
        break;
    }
    case 125:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = EndConfidenceScope();
        break;
    }
    case 126:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = BeginConfidenceScope(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 127:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = UnidentifiedVirtual6(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 128:
    {
        bool arg3 = m_SP[-1] != 0;
        float arg2 = ((float*)m_SP)[-2];
        float arg1 = ((float*)m_SP)[-3];
        float arg0 = ((float*)m_SP)[-4];
        m_SP -= 3;
        ((float*)m_SP)[-1] = UnidentifiedVirtual7(arg0, arg1, arg2, arg3);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 129:
    {
        float arg1 = ((float*)m_SP)[-1];
        bool arg0 = m_SP[-2] != 0;
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnBool(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 130:
    {
        float arg1 = ((float*)m_SP)[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnPlayer(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 131:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E3958(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 132:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E35D4(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 133:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E3700(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 134:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E8090(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 135:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E382C(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 136:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnFloat_800E82E8(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 137:
    {
        float arg1 = ((float*)m_SP)[-1];
        FuzzyFielderReference* arg0 = (FuzzyFielderReference*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnFielder(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 138:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnFloat_800E8414(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 139:
    {
        float arg1 = ((float*)m_SP)[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnInt_800E81BC(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 140:
    {
        float arg1 = ((float*)m_SP)[-1];
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnVariant(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 141:
    {
        float arg1 = ((float*)m_SP)[-1];
        unsigned long arg0 = (unsigned long)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)FuzzyAIReturnU32(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 142:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        m_SP -= 1;
        fn_80314438(this, arg0);
        break;
    }
    case 143:
    {
        float arg1 = ((float*)m_SP)[-1];
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-2];
        m_SP -= 2;
        fn_80314434(this, arg0, arg1);
        break;
    }
    case 144:
    {
        bool arg1 = m_SP[-1] != 0;
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800E8CAC(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 145:
    {
        bool arg1 = m_SP[-1] != 0;
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_80314444(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 146:
    {
        bool arg1 = m_SP[-1] != 0;
        void* arg0 = (void*)m_SP[-2];
        m_SP -= 1;
        m_SP[-1] = (u32)fn_8031443C(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 147:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = FuzzyEqual(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 148:
    {
        ++m_SP;
        m_SP[-1] = (u32)EndActionQueue();
        break;
    }
    case 149:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = BeginActionQueue();
        break;
    }
    case 150:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = FuzzyGetQueueConfidence(this);
        break;
    }
    case 151:
    {
        ++m_SP;
        m_SP[-1] = (u32)FuzzyAIGetBall();
        break;
    }
    case 152:
    {
        ++m_SP;
        m_SP[-1] = (u32)FuzzyAIGetBallOwner();
        break;
    }
    case 153:
    {
        ++m_SP;
        m_SP[-1] = (u32)FuzzyAIGetGame();
        break;
    }
    case 154:
    {
        float arg1 = ((float*)m_SP)[-1];
        float arg0 = ((float*)m_SP)[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = FLESS(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 155:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAITryCachedPlayerQuestion_800E90EC(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 156:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAITryCachedPlayerQuestion_800E9194(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 157:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAITryCachedTeamQuestion(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 158:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = FuzzyNot(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 159:
    {
        UnidentifiedVariant_80054AB8* arg0 = (UnidentifiedVariant_80054AB8*)m_SP[-1];
        m_SP -= 1;
        AddAction(arg0);
        break;
    }
    case 160:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_803147A4(this);
        break;
    }
    case 161:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)FuzzyGetContextParameter(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 162:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        ((float*)m_SP)[-1] = FuzzyGetTimerSeconds(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 163:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D66A0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 164:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = GoalieOutOfPosition(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 165:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = GoalieType(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 166:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF888(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 167:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D763C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 168:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D76B8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 169:
    {
        const char* arg0 = (const char*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAIStringHash(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 170:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = High(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 171:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7734(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 172:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InBetweenMyNetAnd(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 173:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DC19C(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 174:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DBEF4(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 175:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DC434(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 176:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = Incapacitated(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 177:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InControlOfBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 178:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = InDefensiveZone(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 179:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InDefensiveZoneOfPlayer(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 180:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InFrontOfMyNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 181:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InFrontOfTheirNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 182:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA518(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 183:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800E0470(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 184:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800E05A4(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 185:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = InOffensiveZone(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 186:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = InOffensiveZoneOfPlayer(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 187:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = InPassingLane(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 188:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D74D8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 189:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_803144BC(arg0, arg1, arg2);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 190:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_803144C8(arg0, arg1, arg2);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 191:
    {
        float arg4 = ((float*)m_SP)[-1];
        float arg3 = ((float*)m_SP)[-2];
        float arg2 = ((float*)m_SP)[-3];
        float arg1 = ((float*)m_SP)[-4];
        float arg0 = ((float*)m_SP)[-5];
        m_SP -= 4;
        ((float*)m_SP)[-1] = fn_80314504(arg0, arg1, arg2, arg3, arg4);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 192:
    {
        float arg4 = ((float*)m_SP)[-1];
        float arg3 = ((float*)m_SP)[-2];
        float arg2 = ((float*)m_SP)[-3];
        float arg1 = ((float*)m_SP)[-4];
        float arg0 = ((float*)m_SP)[-5];
        m_SP -= 4;
        ((float*)m_SP)[-1] = fn_80314538(arg0, arg1, arg2, arg3, arg4);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 193:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Invincible(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 194:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9D04(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 195:
    {
        ++m_SP;
        m_SP[-1] = (u32)fn_80314798(this);
        break;
    }
    case 196:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)FuzzyIsTimerRunning(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 197:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6AF0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 198:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = LastBallOwner(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 199:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        int arg0 = (int)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D79F4(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 200:
    {
        int arg1 = (int)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D8834(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 201:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Loose(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 202:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Losing(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 203:
    {
        void* arg0 = (void*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D673C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 204:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cFielder* arg0 = (cFielder*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = Marking(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 205:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6734(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 206:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Midfield(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 207:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Moderate(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 208:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = NearTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 209:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 210:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD494(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 211:
    {
        cFielder* arg1 = (cFielder*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DD5C4(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 212:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToFormationPosition(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 213:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToMyGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 214:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToMyNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 215:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9368(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 216:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD234(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 217:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD31C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 218:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D91BC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 219:
    {
        cTeam* arg1 = (cTeam*)m_SP[-1];
        cBall* arg0 = (cBall*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE8CC(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 220:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToTheirGoalie(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 221:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = NearToTheirNet(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 222:
    {
        float arg2 = ((float*)m_SP)[-1];
        float arg1 = ((float*)m_SP)[-2];
        float arg0 = ((float*)m_SP)[-3];
        m_SP -= 2;
        ((float*)m_SP)[-1] = fn_80314448(arg0, arg1, arg2);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 223:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Offensive(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 224:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D78C4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 225:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DACF4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 226:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DAD3C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 227:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA91C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 228:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF028(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 229:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DED3C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 230:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D6D14(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 231:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6D78(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 232:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800D6CD4(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 233:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = OnTheGround(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 234:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBAB0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 235:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = OpenTo(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 236:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = LikelyToScore(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 237:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBB88(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 238:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D66C4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 239:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAICreateOpponentFielderIterator(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 240:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6688(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 241:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE71C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 242:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = Ownerless(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 243:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE0();
        break;
    }
    case 244:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE4();
        break;
    }
    case 245:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Passive(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 246:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF2C0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 247:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6744(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 248:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FE8();
        break;
    }
    case 249:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D782C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 250:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D7878(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 251:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9480(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 252:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D88B4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 253:
    {
        float arg0 = ((float*)m_SP)[-1];
        ((float*)m_SP)[-1] = RandomChance(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            ((float*)m_SP)[-1] = arg0;
        }
        break;
    }
    case 254:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = ReallyCloseToBall(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 255:
    {
        cBall* arg0 = (cBall*)m_SP[-1];
        ((float*)m_SP)[-1] = ReallyHigh(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 256:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF118(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 257:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF474(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 258:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = ReceivingPass(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 259:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF1B8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 260:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF0B8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 261:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DF390(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 262:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9D78(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 263:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = GenerateFilteredRandom();
        break;
    }
    case 264:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6BD8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 265:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = SeparatingFrom(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 266:
    {
        cBall* arg1 = (cBall*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = SeparatingFrom(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 267:
    {
        int arg0 = (int)m_SP[-1];
        m_SP -= 1;
        FuzzySetActionSelection(this, arg0);
        break;
    }
    case 268:
    {
        const char* arg1 = (const char*)m_SP[-1];
        UnidentifiedTransitionReference* arg0 = (UnidentifiedTransitionReference*)m_SP[-2];
        m_SP -= 2;
        FuzzySetTransition(this, arg0, arg1);
        break;
    }
    case 269:
    {
        const char* arg1 = (const char*)m_SP[-1];
        UnidentifiedScriptMachine* arg0 = (UnidentifiedScriptMachine*)m_SP[-2];
        m_SP -= 2;
        FuzzyAISetTransition(arg0, arg1);
        break;
    }
    case 270:
    {
        float arg1 = ((float*)m_SP)[-1];
        unsigned long arg0 = (unsigned long)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = FuzzySetTimerSeconds(this, arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 271:
    {
        ++m_SP;
        ((float*)m_SP)[-1] = fn_800E3FEC();
        break;
    }
    case 272:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        ((float*)m_SP)[-1] = GetSkillValue(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 273:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D77B0(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 274:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Stalling(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 275:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D6A90(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 276:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        m_SP[-1] = (u32)fn_800DF790(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 277:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = StrategicBallOwner(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 278:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D8970(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 279:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Striker(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 280:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DA0C8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 281:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD7F4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 282:
    {
        Goalie* arg0 = (Goalie*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DE7D8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 283:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD944(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 284:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DD99C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 285:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)FuzzyAICreateTeammateIterator(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 286:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        m_SP[-1] = (u32)fn_800D6670(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 287:
    {
        char arg0 = (char)m_SP[-1];
        m_SP[-1] = (unsigned char)fn_80312358(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (unsigned char)arg0;
        }
        break;
    }
    case 288:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Tied(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 289:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeCloseToOver(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 290:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeFarFromOver(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 291:
    {
        cGame* arg0 = (cGame*)m_SP[-1];
        ((float*)m_SP)[-1] = TimeNearlyOver(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 292:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800D9DD8(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 293:
    {
        cPlayer* arg1 = (cPlayer*)m_SP[-1];
        cPlayer* arg0 = (cPlayer*)m_SP[-2];
        m_SP -= 1;
        ((float*)m_SP)[-1] = fn_800DE40C(arg0, arg1);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 294:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = UserControlled(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 295:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = UserControlledT(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 296:
    {
        unsigned long arg0 = (unsigned long)m_SP[-1];
        m_SP[-1] = (u32)FuzzyWasTimerRunning(this, arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 297:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DBB0C(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 298:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEB04(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 299:
    {
        cPlayer* arg0 = (cPlayer*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEBBC(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 300:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEAB4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 301:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEBF4(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 302:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = fn_800DEC88(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 303:
    {
        cFielder* arg0 = (cFielder*)m_SP[-1];
        ((float*)m_SP)[-1] = Winger(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    case 304:
    {
        cTeam* arg0 = (cTeam*)m_SP[-1];
        ((float*)m_SP)[-1] = Winning(arg0);
        if (FuzzyAIIsUndoingCall(this))
        {
            m_SP[-1] = (u32)arg0;
        }
        break;
    }
    default:
        nlBreak();
        break;
    }
}

extern "C" bool FuzzyAIIsUndoingCall(InterpreterCore* value)
{
    return value->m_RunState == 3;
}

extern "C" float fn_800E7ECC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" unsigned long fn_800E7ED4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EDC(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" unsigned long fn_800E7EE4(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" float fn_800E7EEC(void*, Variant* value)
{
    return value->mData.f;
}

extern "C" float FuzzyAIGetConfidence(
    void*, UnidentifiedVariant_80054AB8* value)
{
    if (value->ExtraData.IsSet(4))
    {
        return value->ExtraData.Get(4)->mData.f;
    }

    return 0.0f;
}

extern "C" float FuzzyAIBoolToFloat(bool value)
{
    return value ? 1.0f : 0.0f;
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnBool(
    FuzzyAIRuntime* runtime, bool value, float confidence)
{
    return runtime->CreateReturnValue(FT_BOOL, value, confidence);
}

UnidentifiedVariant_80054AB8* FuzzyRuntimeBase::ReturnValue(
    UnidentifiedVariant_80054AB8* value, float)
{
    return value;
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E8090(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnInt_800E81BC(
    FuzzyAIRuntime* runtime, int value, float confidence)
{
    return runtime->CreateReturnValue(FT_INT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFloat_800E82E8(
    FuzzyAIRuntime* runtime, float value, float confidence)
{
    return runtime->CreateReturnValue(FT_FLOAT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFloat_800E8414(
    FuzzyAIRuntime* runtime, float value, float confidence)
{
    return runtime->CreateReturnValue(FT_FLOAT, value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnVariant(
    FuzzyAIRuntime* runtime,
    UnidentifiedVariant_80054AB8* value, float confidence)
{
    return runtime->CreateReturnValue(value, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnU32(
    FuzzyAIRuntime* runtime, unsigned long value, float confidence)
{
    return runtime->CreateReturnValue(FT_U32, value, confidence);
}

extern "C" float fn_800E8CAC(void* runtime, float value, bool flag)
{
    return fn_80314444(runtime, value, flag);
}

extern "C" unsigned long fn_800E8CB0(void*, Variant* value)
{
    return value->mData.u;
}

extern "C" void FuzzyAISetPlayerParameter_800E8CB8(
    void*, cPlayer* value, unsigned long parameterHash,
    UnidentifiedVariant_80054AB8* action)
{
    int index = FuzzyFindParameterIndex(parameterHash);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    action->ExtraData.Set(index, variant);
}

extern "C" void FuzzyAISetIntParameter_800E8D68(FuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzySetIntParameter(runtime, value, hash, action);
}

extern "C" void FuzzyAISetIntParameter_800E8D6C(FuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzySetIntParameter(runtime, value, hash, action);
}

extern "C" void FuzzyAISetIntParameter_800E8D70(FuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzySetIntParameter(runtime, value, hash, action);
}

extern "C" void FuzzyAISetIntParameter_800E8D74(FuzzyRuntimeBase* runtime, int value, unsigned long hash, UnidentifiedVariant_80054AB8* action)
{
    FuzzySetIntParameter(runtime, value, hash, action);
}

extern "C" void FuzzyAISetFielderParameter(
    void*, FuzzyFielderReference* value,
    unsigned long parameterHash, UnidentifiedVariant_80054AB8* action)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    int index = FuzzyFindParameterIndex(parameterHash);
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = fielder;
    action->ExtraData.Set(index, variant);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnPlayer(
    FuzzyAIRuntime* runtime, cPlayer* value, float confidence)
{
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(value);
    result->SetParameter(4, FuzzyVariant(confidence));
    runtime->mReturnInstructionOffset = runtime->GetInstructionOffset() + 1;
    return runtime->ReturnValue(result, confidence);
}

extern "C" UnidentifiedVariant_80054AB8* FuzzyAIReturnFielder(
    FuzzyAIRuntime* runtime,
    FuzzyFielderReference* value, float confidence)
{
    cFielder* fielder = value->mTeam->GetFielder(value->mIndex);
    UnidentifiedVariant_80054AB8* result = new (lbl_805842C8.Allocate())
        UnidentifiedVariant_80054AB8(fielder);
    result->SetParameter(4, FuzzyVariant(confidence));
    runtime->mReturnInstructionOffset = runtime->GetInstructionOffset() + 1;
    return runtime->ReturnValue(result, confidence);
}

extern "C" bool FuzzyAITryCachedPlayerQuestion_800E90EC(FuzzyRuntimeBase* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return FuzzyTryCachedQuestion(runtime, variant);
}

extern "C" bool FuzzyAITryCachedPlayerQuestion_800E9194(FuzzyRuntimeBase* runtime, cPlayer* value)
{
    FuzzyVariant variant;
    variant.mType = FT_PLAYER;
    variant.mData.pPlayer = value;
    return FuzzyTryCachedQuestion(runtime, variant);
}

extern "C" bool FuzzyAITryCachedTeamQuestion(FuzzyRuntimeBase* runtime, cTeam* value)
{
    FuzzyVariant variant;
    variant.mType = FT_TEAM;
    variant.mData.pTeam = value;
    return FuzzyTryCachedQuestion(runtime, variant);
}

extern "C" void FuzzyAISetTransition(
    UnidentifiedScriptMachine* state, const char* name)
{
    ScriptTransitionFunc value(name);
    state->mTransition.mFuncHash = value.mFuncHash;
    state->mTransition.mNativeFunc = value.mNativeFunc;
}
