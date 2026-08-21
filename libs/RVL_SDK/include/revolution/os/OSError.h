#ifndef REVOLUTION_OS_ERROR_H
#define REVOLUTION_OS_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

#define OSAssert(file, line, expression, ...) \
    if (!(expression))                        \
    {                                         \
        OSPanic(file, line, __VA_ARGS__);     \
    }

    void OSPanic(const char* file, int line, const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_OS_ERROR_H
