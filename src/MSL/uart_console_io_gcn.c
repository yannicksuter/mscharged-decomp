#include "uart_console_io_gcn.h"
#include <revolution/euart.h>
#include <revolution/os.h>

DSIOResult __write_console(__file_handle handle, unsigned char* buffer, size_t* count, __idle_proc idle_fn)
{
    static int initialized;

    if ((OSGetConsoleType() & 0x20000000) == 0)
    {
        int uart_init = 0;
        if (initialized == FALSE)
        {
            uart_init = InitializeUART(0xE100);

            if (uart_init == 0)
            {
                initialized = 1;
            }
        }

        if (uart_init != 0)
        {
            return 1;
        }

        if (WriteUARTN((const char*)buffer, *count) != 0)
        {
            *count = 0;
            return 1;
        }
    }

    __TRK_write_console(handle, buffer, count, idle_fn);
    return 0;
}
