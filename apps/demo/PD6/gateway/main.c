#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

#define PACKET_CACHE_SIZE 30

typedef struct
{
    uint16_t sourceId;
    uint32_t seq;
} PacketCache_t;

static PacketCache_t packetCache[PACKET_CACHE_SIZE];
static uint8_t cacheIndex = 0;
static uint8_t radioBuffer[RADIO_MAX_PACKET];
static uint32_t totalReceived = 0;

static bool isPacketSeen(uint16_t sourceId, uint32_t seq)
{
    uint8_t i;
    for (i = 0; i < PACKET_CACHE_SIZE; i++)
    {
        if (packetCache[i].sourceId == sourceId &&
            packetCache[i].seq == seq)
        {
            return true;
        }
    }
    return false;
}

static void addPacketToCache(uint16_t sourceId, uint32_t seq)
{
    packetCache[cacheIndex].sourceId = sourceId;
    packetCache[cacheIndex].seq = seq;
    cacheIndex = (cacheIndex + 1) % PACKET_CACHE_SIZE;
}

static void recvRadio(void)
{
    int16_t len = radioRecv(radioBuffer, sizeof(radioBuffer));
    if (len < 0)
    {
        return;
    }

    if (len != (int16_t)sizeof(Pd6Packet_t))
    {
        return;
    }

    {
        Pd6Packet_t packet;
        int8_t rssi;
        uint8_t lqi;

        memcpy(&packet, radioBuffer, sizeof(packet));

        if (memcmp(packet.key, PD6_KEY_STR, PD6_KEY_LEN) != 0)
        {
            return;
        }

        if (isPacketSeen(packet.sourceId, packet.seq))
        {
            redLedToggle();
            return;
        }

        addPacketToCache(packet.sourceId, packet.seq);

        rssi = radioGetLastRSSI();
        lqi = radioGetLastLQI();
        totalReceived++;

        // CSV: sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived
        PRINTF("%u,%lu,%u,%u,%d,%u,%lu\n",
               packet.sourceId,
               (unsigned long)packet.seq,
               packet.lightValue,
               packet.hopCount,
               (int)rssi,
               (unsigned)lqi,
               (unsigned long)totalReceived);

        greenLedToggle();
    }
}

void appMain(void)
{
    uint8_t i;
    for (i = 0; i < PACKET_CACHE_SIZE; i++)
    {
        packetCache[i].sourceId = 0;
        packetCache[i].seq = 0;
    }

    radioSetReceiveHandle(recvRadio);
    radioOn();

    PRINTF("sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived\n");
    PRINTF("# Gateway started, ID=0x%04x\n", localAddress);

    while (1)
    {
        blueLedToggle();
        mdelay(1000);
    }
}
