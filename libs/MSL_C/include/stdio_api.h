#ifndef MSL_STDIO_API_H
#define MSL_STDIO_API_H

enum __ReadProcActions {
    __GetAChar,
    __UngetAChar,
    __TestForError
};

typedef struct {
    char* NextChar;
    int NullCharDetected;
} __InStrCtrl;

#ifdef __cplusplus
extern "C" {
#endif

int __StringRead(void*, int, int);

#ifdef __cplusplus
}
#endif

#endif // MSL_STDIO_API_H
