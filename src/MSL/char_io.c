#include "buffer_io.h"
#include "char_io.h"
#include "critical_regions.h"
#include "FILE_POS.h"
#include "misc_io.h"
#include "wchar_io.h"

int __get_char(FILE* file)
{
    int io_state;
    int load_result;

    file->buffer_length = 0;

    if (file->file_state.error != 0 || file->file_mode.file_kind == file_closed)
    {
        return EOF;
    }

    io_state = file->file_state.io_state;

    if (io_state == writing || !(file->file_mode.io_mode & __read))
    {
        file->file_state.error = 1;
        file->buffer_length = 0;
        return EOF;
    }
    else if (io_state >= rereading)
    {
        file->file_state.io_state = io_state - 1;

        if (io_state == rereading)
        {
            file->buffer_length = file->save_buffer_length;
        }

        return ((unsigned char*)file)[io_state + 0xC];
    }
    else
    {
        file->file_state.io_state = reading;
        load_result = __load_buffer(file, NULL, __align_buffer);

        if (load_result != __no_io_error || file->buffer_length == 0)
        {
            if (load_result == __io_error)
            {
                file->file_state.error = 1;
                file->buffer_length = 0;
            }
            else
            {
                file->file_state.io_state = neutral;
                file->file_state.eof = 1;
                file->buffer_length = 0;
            }

            return EOF;
        }
        else
        {
            unsigned long length;
            unsigned char* ptr;

            length = file->buffer_length;
            file->buffer_length = length - 1;
            ptr = file->buffer_ptr;
            file->buffer_ptr = ptr + 1;
            return *ptr;
        }
    }
}

int fgetc(FILE* file)
{
    if (__fwide(file, -1) >= 0)
    {
        return EOF;
    }

    return file->buffer_length-- ? *file->buffer_ptr++ : __get_char(file);
}

int __put_char(int c, FILE* stream)
{
    int ret;

    int file_kind = stream->file_mode.file_kind;
    stream->buffer_length = 0;

    if (stream->file_state.error != 0 || file_kind == file_closed)
    {
        return -1;
    }

    if (file_kind == file_console)
    {
        __stdio_atexit();
    }

    if (stream->file_state.io_state == neutral && (stream->file_mode.io_mode & __write))
    {
        if ((stream->file_mode.io_mode & __append) && _fseek(stream, 0, 2) != 0)
        {
            return 0;
        }

        stream->file_state.io_state = writing;
        __prep_buffer(stream);
    }

    if (stream->file_state.io_state != writing)
    {
        stream->file_state.error = 1;
        ret = -1;
        stream->buffer_length = 0;
    }
    else if ((stream->file_mode.buffer_mode == 2 || stream->buffer_size == (unsigned int)stream->buffer_ptr - (unsigned int)stream->buffer) && __flush_buffer(stream, NULL) != 0)
    {
        stream->file_state.error = 1;
        ret = -1;
        stream->buffer_length = 0;
    }
    else
    {
        stream->buffer_length--;
        *stream->buffer_ptr++ = c;

        if (stream->file_mode.buffer_mode != 2)
        {
            if ((stream->file_mode.buffer_mode == 0 || c == 10) && __flush_buffer(stream, NULL) != 0)
            {
                stream->file_state.error = 1;
                ret = -1;
                stream->buffer_length = 0;
                goto exit;
            }
            stream->buffer_length = 0;
        }

        ret = c & 0xFF;
    }

exit:
    return ret;
}

#define __putc(c, file) ((__fwide(file, -1) >= 0) ? -1 : (file)->buffer_length-- ? (int)(*(file)->buffer_ptr++ = (unsigned char)(c)) : __put_char(c, file))

int fputs(const char* s, FILE* stream)
{
    char c;
    int ret = 0;

    __begin_critical_region(stdin_access);
    while (c = *s++, c != 0)
    {
        if (__putc(c, stream) == -1)
        {
            ret = -1;
            break;
        }
    }
    __end_critical_region(stdin_access);

    return ret;
}
