#include <revolution/os.h>

extern s32 NCDiGetEnabledConfigList(u32* list0, u32* list1, u32* list2);

s32 GetStartupErrorCode(s32 error, s32 connectionType);

s32 NETiGetConnectionTypeFromConfigList(u32 list0, u32 list1, u32 list2)
{
    s32 result = 99;
    s32 i;
    u32 bit;

    if (list0 != 0)
    {
        if (list1 == 0 && list2 == 0)
        {
            bit = 1;
            for (i = 0; i < 32; i++)
            {
                if (list0 & bit)
                {
                    break;
                }
                bit <<= 1;
            }
            if (i == 32)
            {
                i = -1;
            }
            result = i + 20;
        }
    }
    else if (list1 != 0)
    {
        if (list2 == 0)
        {
            bit = 1;
            for (i = 0; i < 32; i++)
            {
                if (list1 & bit)
                {
                    break;
                }
                bit <<= 1;
            }
            if (i == 32)
            {
                i = -1;
            }
            result = i + 30;
        }
    }
    else if (list2 != 0)
    {
        bit = 1;
        for (i = 0; i < 32; i++)
        {
            if (list2 & bit)
            {
                break;
            }
            bit <<= 1;
        }
        if (i == 32)
        {
            i = -1;
        }
        result = i + 40;
    }

    return result;
}

s32 NETGetStartupErrorCode(s32 error)
{
    u32 list2;
    u32 list1;
    u32 list0;
    s32 connectionType = 99;

    if (NCDiGetEnabledConfigList(&list0, &list1, &list2) >= 0)
    {
        connectionType = NETiGetConnectionTypeFromConfigList(list0, list1, list2);
    }
    if (connectionType < 0)
    {
        error = 0x80000000;
        connectionType = 99;
    }

    return GetStartupErrorCode(error, connectionType) - connectionType;
}

s32 GetStartupErrorCode(s32 error, s32 connectionType)
{
    if (error >= 0)
    {
        return 0;
    }

    switch (error)
    {
    case -45:
        return -50200;
    case -28:
        return -50300;
    case -62:
        return -50400;
    case -111:
        return -52700;
    case -121:
        if (connectionType >= 20 && connectionType < 30)
        {
            return -51400;
        }
        return -51000;
    case -112:
    case -76:
    case -48:
    case -39:
        if (connectionType >= 20 && connectionType < 30)
        {
            return -51400;
        }
        return -51300;
    case -102:
    case -101:
    case -100:
        return -52000;
    case (s32)0x80000000:
        return -50100;
    default:
        OSReport("Unknown SOStartup Error: %d\n", error);
        return -50100;
    }
}
