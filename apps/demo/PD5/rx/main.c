#include "stdmansos.h"
#include <string.h>

#include "pd5_packet.h"

static uint8_t radioBuffer[RADIO_MAX_PACKET];

static volatile uint32_t g_rxCount;

static void recvRadio(void)
{
    int16_t len = radioRecv(radioBuffer, sizeof(radioBuffer));
    if (len < 0) {
        return;
    }

    if (len != (int16_t)sizeof(Pd5Packet_t)) {
        return;
    }

    {
        Pd5Packet_t packet;
        int8_t rssi;
        uint8_t lqi;

        memcpy(&packet, radioBuffer, sizeof(packet));
        if (memcmp(packet.key, PD5_KEY_STR, PD5_KEY_LEN) != 0) {
            return;
        }
        rssi = radioGetLastRSSI();
        lqi = radioGetLastLQI();

        g_rxCount++;

        // CSV-style output: seq,rssi,lqi,rxCount
        PRINTF("%lu,%d,%u,%lu\n",
               (unsigned long)packet.seq,
               (int)rssi,
               (unsigned)lqi,
               (unsigned long)g_rxCount);

        greenLedToggle();
    }
}

void appMain(void)
{
    radioSetReceiveHandle(recvRadio);
    radioOn();

    // Print header once; then each received packet is one CSV line.
    PRINTF("seq,rssi,lqi,rxCount\n");

    while (1) {
        blueLedToggle();
        mdelay(1000);
    }
}
