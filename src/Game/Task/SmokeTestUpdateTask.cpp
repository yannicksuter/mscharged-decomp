#include "Game/Task/SmokeTestUpdateTask.h"

#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebugFile.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "unclassified/tu_802BAE84.h"

#include <stdarg.h>

bool lbl_806E1DF0;
void (*lbl_806E1DF4)();
void (*lbl_806E1DF8)(float);

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
    if (!lbl_806E1DF0)
    {
        return;
    }

    if (mComplete)
    {
        fn_802BB048(0, 0, 0, 4, sSmokeTestCompleted, mDuration - mElapsed);
        return;
    }

    if (mElapsed > mDuration)
    {
        const char* smokeTestName = GetTweakString(sSmokeTestNamePath, sNotFound);
        fn_802BD644(sSmokeTestNameFormat, smokeTestName);

        if (lbl_806E1DF4 != 0)
        {
            lbl_806E1DF4();
        }

        void* file = nlOpenFileDebug(sProfilePath, false, false);
        nlWriteLineDebug(file, "\n", false);
        nlCloseFileDebug(file);
        mComplete = true;
        return;
    }

    fn_802BB048(0, 0, 0, 4, sSmokeTestRunning, mDuration - mElapsed);
    mElapsed += dt;

    if (lbl_806E1DF8 != 0)
    {
        lbl_806E1DF8(dt);
    }
}

bool fn_802BD63C()
{
    return lbl_806E1DF0;
}

void fn_802BD644(const char* format, ...)
{
    va_list args;

    if (lbl_806E1DF0)
    {
        va_start(args, format);
        nlVSNPrintf(sSmokeLogBuffer, sizeof(sSmokeLogBuffer), format, args);

        void* file = nlOpenFileDebug(sSmokeLogPath, false, true);
        nlWriteLineDebug(file, sSmokeLogBuffer, false);
        nlCloseFileDebug(file);
        va_end(args);
    }
}

void fn_802BD718(const char* name, const char* units, float value)
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
    fn_802BD644(output);
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

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

char sSmokeLogBuffer[0x200];
