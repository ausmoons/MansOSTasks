#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

#define PACKET_CACHE_SIZE 20

typedef struct
{
    uint16_t sourceId;
    uint32_t seq;
} PacketCache_t;

static PacketCache_t packetCache[PACKET_CACHE_SIZE];
static uint8_t cacheIndex = 0;
static uint8_t radioBuffer[RADIO_MAX_PACKET];

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
        memcpy(&packet, radioBuffer, sizeof(packet));

        if (memcmp(packet.key, PD6_KEY_STR, PD6_KEY_LEN) != 0)
        {
            return;
        }

        if (isPacketSeen(packet.sourceId, packet.seq))
        {
            yellowLedToggle();
            PRINTF("Relay: Duplicate ignored - ID=0x%04x seq=%lu\n",
                   packet.sourceId,
                   (unsigned long)packet.seq);
            return;
        }

        addPacketToCache(packet.sourceId, packet.seq);

        packet.hopCount++;
        radioSend(&packet, sizeof(packet));
        greenLedToggle();

        PRINTF("Relay: Forwarded - ID=0x%04x seq=%lu light=%u hops=%u\n",
               packet.sourceId,
               (unsigned long)packet.seq,
               packet.lightValue,
               packet.hopCount);
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

    PRINTF("Relay started, ID=0x%04x\n", localAddress);

    while (1)
    {
        blueLedToggle();
        mdelay(2000);
    }
}
