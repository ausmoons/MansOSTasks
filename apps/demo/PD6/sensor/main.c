#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

#define SEND_INTERVAL_MS 5000

void appMain(void)
{
    Pd6Packet_t packet;
    uint32_t seq = 0;

    memcpy(packet.key, PD6_KEY_STR, PD6_KEY_LEN);
    packet.sourceId = localAddress;
    packet.hopCount = 0;

    lightOn();
    radioOn();

    PRINTF("Sensor started, ID=0x%04x\n", localAddress);

    while (1)
    {
        packet.lightValue = lightRead();
        packet.seq = ++seq;

        radioSend(&packet, sizeof(packet));
        redLedToggle();

        PRINTF("Sent: ID=0x%04x seq=%lu light=%u\n",
               packet.sourceId,
               (unsigned long)packet.seq,
               packet.lightValue);

        mdelay(SEND_INTERVAL_MS);
    }
}
