#include "NL/nlTask.h"
#include "NL/nlPrint.h"

#include "types.h"

typedef struct __va_list_struct
{
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];

typedef __va_list va_list;

extern void __builtin_va_info(void*);
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap)        ((void)0)

class SmokeTestUpdateTask : public nlTask
{
public:
    SmokeTestUpdateTask()
        : mComplete(false)
        , mElapsed(0.0f)
        , mDuration(1.0f)
    {
    }

    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "SmokeTest Update";
    }

private:
    bool mComplete;
    float mElapsed;
    float mDuration;
};

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

extern "C" bool fn_802BD63C();
extern "C" void fn_802BD644(const char* format, ...);
extern "C" void fn_802BD718(const char* name, const char* units, float value);

extern "C" void fn_802BB048(void*, void*, void*, int, const char*, ...);
extern "C" const char* fn_802C2D20(const char*, const char*);
extern "C" void* fn_802B18E8(const char*, bool, bool);
extern "C" void fn_802B19BC(void*, const char*, bool);
extern "C" void fn_802B1A1C(void*);
extern "C" void fn_802AA9D8(char*, u32, const char*, va_list);
extern "C" void fn_8000C280(char*, const char*, const char*, u32);

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
        const char* smokeTestName = fn_802C2D20(sSmokeTestNamePath, sNotFound);
        fn_802BD644(sSmokeTestNameFormat, smokeTestName);

        if (lbl_806E1DF4 != 0)
        {
            lbl_806E1DF4();
        }

        void* file = fn_802B18E8(sProfilePath, false, false);
        fn_802B19BC(file, "\n", false);
        fn_802B1A1C(file);
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

extern "C" bool fn_802BD63C()
{
    return lbl_806E1DF0;
}

extern "C" void fn_802BD644(const char* format, ...)
{
    va_list args;

    if (lbl_806E1DF0)
    {
        va_start(args, format);
        fn_802AA9D8(sSmokeLogBuffer, sizeof(sSmokeLogBuffer), format, args);

        void* file = fn_802B18E8(sSmokeLogPath, false, true);
        fn_802B19BC(file, sSmokeLogBuffer, false);
        fn_802B1A1C(file);
        va_end(args);
    }
}

extern "C" void fn_802BD718(const char* name, const char* units, float value)
{
    char output[0x400];
    char unitsOutput[0x100];

    output[0] = '\0';
    nlSNPrintf(output, sizeof(output), sGraphValueFormat, name, value);

    if (units != 0)
    {
        nlSNPrintf(unitsOutput, sizeof(unitsOutput), sGraphUnitsFormat, units);
        fn_8000C280(output, output, unitsOutput, sizeof(output));
    }

    fn_8000C280(output, output, "\n", sizeof(output));
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

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

char sSmokeLogBuffer[0x200];
