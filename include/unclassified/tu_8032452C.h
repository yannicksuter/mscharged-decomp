#ifndef UNCLASSIFIED_TU_8032452C_H
#define UNCLASSIFIED_TU_8032452C_H

#include "types.h"

class UnidentifiedReliableSocketCallback;
struct UnidentifiedReliableSocketState;

extern "C"
{
    int fn_80324778(UnidentifiedReliableSocketState* socket,
        UnidentifiedReliableSocketCallback* callback);
    void fn_80324828(UnidentifiedReliableSocketState* socket);
    void fn_80324918(
        UnidentifiedReliableSocketState* socket, bool enabled);
    int fn_80324920(UnidentifiedReliableSocketState* socket,
        void* connection, const u8* address, u16 port);
    void fn_80324A1C(
        UnidentifiedReliableSocketState* socket, void* a, void* b);
    void fn_80324A28(UnidentifiedReliableSocketState* socket, int aid,
        void* buffer, int size, bool reliable);
    void fn_80324A4C(UnidentifiedReliableSocketState* socket, u8 aid,
        void* buffer, int size);
    void fn_80324A5C(UnidentifiedReliableSocketState* socket, void* a,
        void* b, bool c);
    void fn_80324B54(UnidentifiedReliableSocketState* socket);
    void fn_80324D1C(UnidentifiedReliableSocketState* socket);
    void fn_80325264(UnidentifiedReliableSocketState* socket, void* buffer,
        int size, const u8* address, u16 port);
    void* fn_80325388(
        UnidentifiedReliableSocketState* socket, const u8* address);
    void fn_80325404(
        UnidentifiedReliableSocketState* socket, u32 connection);
    void fn_8032540C(
        UnidentifiedReliableSocketState* socket, u32 connection);
}

#endif // UNCLASSIFIED_TU_8032452C_H
