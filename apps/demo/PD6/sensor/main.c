#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

// Sensor node configuration
// Each sensor must have a unique ID
// You can change this when compiling: make telosb CONST=SENSOR_ID=2
#ifndef SENSOR_ID
#define SENSOR_ID 1
#endif

#define SEND_INTERVAL_MS 5000  // Send packet every 5 seconds

void appMain(void)
{
    Pd6Packet_t packet;
    uint32_t seq = 0;

    // Initialize packet with network key
    memcpy(packet.key, PD6_KEY_STR, PD6_KEY_LEN);
    packet.sourceId = SENSOR_ID;
    packet.hopCount = 0;  // Source node starts at hop 0

    // Turn on light sensor
    lightSensorOn();

    // Turn on radio
    radioOn();

    PRINTF("Sensor Node ID=%u started\n", SENSOR_ID);

    while (1) {
        // Read light sensor value
        packet.lightValue = lightSensorRead();

        // Increment sequence number
        packet.seq = ++seq;

        // Send packet via radio
        radioSend(&packet, sizeof(packet));

        // Visual feedback
        redLedToggle();

        PRINTF("Sent: ID=%u seq=%lu light=%u\n",
               packet.sourceId,
               (unsigned long)packet.seq,
               packet.lightValue);

        // Wait before next transmission
        mdelay(SEND_INTERVAL_MS);
    }
}
