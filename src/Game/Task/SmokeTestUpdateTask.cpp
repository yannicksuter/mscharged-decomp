#include "Game/Task/SmokeTestUpdateTask.h"

#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebugFile.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/Sys/tweak.h"

#include <stdarg.h>

bool g_bSmokeTestEnabled;
void (*g_pSmokeTestFinishedCallback)();
void (*g_pSmokeTestUpdateCallback)(float);

extern char sSmokeLogPathString[];
extern char sSmokeTestCompleted[];
extern char sSmokeTestNamePath[];
extern char sNotFound[];
extern char sSmokeTestNameFormat[];
extern char sProfilePath[];
extern char sSmokeTestRunning[];
extern char sGraphValueFormat[];
extern char sGraphUnitsFormat[];
extern char sSmokeLogBuffer[0x200];

const char* sSmokeLogPath = sSmokeLogPathString;

void SmokeTestUpdateTask::Run(float dt)
{
    if (!g_bSmokeTestEnabled)
    {
        return;
    }

    if (mComplete)
    {
        nlScreenPrintf(0, 0, 0, 4, sSmokeTestCompleted, mDuration - mElapsed);
        return;
    }

    if (mElapsed > mDuration)
    {
        const char* smokeTestName = GetTweakString(sSmokeTestNamePath, sNotFound);
        SmokeTestLog(sSmokeTestNameFormat, smokeTestName);

        if (g_pSmokeTestFinishedCallback != 0)
        {
            g_pSmokeTestFinishedCallback();
        }

        void* file = nlOpenFileDebug(sProfilePath, false, false);
        nlWriteLineDebug(file, "\n", false);
        nlCloseFileDebug(file);
        mComplete = true;
        return;
    }

    nlScreenPrintf(0, 0, 0, 4, sSmokeTestRunning, mDuration - mElapsed);
    mElapsed += dt;

    if (g_pSmokeTestUpdateCallback != 0)
    {
        g_pSmokeTestUpdateCallback(dt);
    }
}

bool IsSmokeTestEnabled()
{
    return g_bSmokeTestEnabled;
}

void SmokeTestLog(const char* format, ...)
{
    va_list args;

    if (g_bSmokeTestEnabled)
    {
        va_start(args, format);
        nlVSNPrintf(sSmokeLogBuffer, sizeof(sSmokeLogBuffer), format, args);

        void* file = nlOpenFileDebug(sSmokeLogPath, false, true);
        nlWriteLineDebug(file, sSmokeLogBuffer, false);
        nlCloseFileDebug(file);
        va_end(args);
    }
}

void SmokeTestLogGraphValue(const char* name, const char* units, float value)
{
    char output[0x400];
    char unitsOutput[0x100];

    output[0] = '\0';
    nlSNPrintf(output, sizeof(output), sGraphValueFormat, name, value);

    if (units != 0)
    {
        nlSNPrintf(unitsOutput, sizeof(unitsOutput), sGraphUnitsFormat, units);
        nlStrNCat(output, output, unitsOutput, sizeof(output));
    }

    nlStrNCat(output, output, "\n", sizeof(output));
    SmokeTestLog(output);
}

char sSmokeLogPathString[] = "..\\smokelog.txt";
char sSmokeTestCompleted[] = "Smoke Test Completed.";
char sSmokeTestNamePath[] = "/User/SmokeTestName";
char sNotFound[] = "Not Found";
char sSmokeTestNameFormat[] = "SmokeTestName = %s";
char sProfilePath[] = "..\\profile.txt";
char sSmokeTestRunning[] = "Smoke Test Running.  %0.2f sec until end of test.";
char sGraphValueFormat[] = "\nGRAPHVALUE \"%s\"=%f";
char sGraphUnitsFormat[] = " UNITS=\"%s\"";

static SmokeTestUpdateTask sSmokeTestUpdateTask;

char sSmokeLogBuffer[0x200];
