#ifndef MSL_ANSI_FILES_H
#define MSL_ANSI_FILES_H

#include <MetroTRK/trkenum.h>
#include <revolution/types.h>
#include <size_t.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef u32 __file_handle;
    typedef u32 fpos_t;
    typedef void (*__idle_proc)(void);

    typedef struct _file_modes
    {
        unsigned int open_mode : 2;
        unsigned int io_mode : 3;
        unsigned int buffer_mode : 2;
        unsigned int file_kind : 3;
        unsigned int file_orientation : 2;
        unsigned int binary_io : 1;
    } file_modes;

    DSIOResult __position_file(__file_handle handle, fpos_t* position, int mode, __idle_proc idle_proc);
    DSIOResult __close_file(__file_handle file);
    DSIOResult __open_file(const char* name, file_modes mode, __file_handle* handle);
    DSIOResult __write_file(__file_handle file, unsigned char* buf, size_t* count, __idle_proc idle_fn);
    DSIOResult __read_file(__file_handle file, unsigned char* buf, size_t* count, __idle_proc idle_fn);
    DSIOResult __close_console(__file_handle file);
    DSIOResult __TRK_write_console(__file_handle file, unsigned char* buffer, size_t* count, __idle_proc idle_fn);
    DSIOResult __read_console(__file_handle file, unsigned char* buffer, size_t* count, __idle_proc idle_fn);
    void __close_all(void);

#ifdef __cplusplus
}
#endif

#endif // MSL_ANSI_FILES_H
