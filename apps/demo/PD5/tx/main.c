#include "stdmansos.h"
#include <string.h>

#include "pd5_packet.h"

void appMain(void)
{
    Pd5Packet_t packet;
    uint32_t seq = 0;

    memcpy(packet.key, PD5_KEY_STR, PD5_KEY_LEN);

    radioOn();

    while (1)
    {
        packet.seq = ++seq;
        radioSend(&packet, sizeof(packet));
        redLedToggle();
        mdelay(200);
    }
}
