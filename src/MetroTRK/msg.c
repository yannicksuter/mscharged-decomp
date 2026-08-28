#include <MetroTRK/msg.h>
#include <MetroTRK/trk.h>

DSError TRKMessageSend(TRK_Msg* msg)
{
    DSError write_err = TRKWriteUARTN(&msg->m_msg, msg->m_msgLength);
    return kNoError;
}
